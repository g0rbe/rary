#include "rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

char *RARY_SYSTEMD_unit_dirs[] = {
    "/etc/systemd/system.control",
    "/run/systemd/system.control",
    "/run/systemd/transient",
    "/run/systemd/generator.early",
    "/etc/systemd/system",
    "/etc/systemd/systemd.attached",
    "/run/systemd/system",
    "/run/systemd/systemd.attached",
    "/run/systemd/generator",
    "/usr/lib/systemd/system",
    "/run/systemd/generator.late",
    "/etc/systemd/user",
    "/run/systemd/user",
    "/usr/lib/systemd/user",
    NULL
};

int RARY_SYSTEMD_is_unit_exist(const char *name)
{
    DIR *dirp = NULL;
    struct dirent *direntp = NULL;

    // Iterate over `RARY_SYSTEMD_unit_dirs`
    for (char **dirs_path = RARY_SYSTEMD_unit_dirs; *dirs_path != NULL; dirs_path++) {

        // Not every directory is available in every system
        // If the direcory is not exist, simply skip it
        if (access(*dirs_path, R_OK) != 0) {   
            continue;
        }

        if ((dirp = opendir(*dirs_path)) == NULL) {
            fprintf(stderr, "Failed to open %s: %s\n", *dirs_path, strerror(errno));
            return -1;
        }

        // Reset errno to 0, remove the previous errno from access()
        errno = 0;

        while ((direntp = readdir(dirp)) != NULL) {

            if (strcmp(name, direntp->d_name) == 0) {
                closedir(dirp);
                return 1;
            }
        }

        // If errno is not 0, then the NULL from readdir() indicates an error
        if (errno != 0) {
            fprintf(stderr, "Failed to read directory %s: %s\n", *dirs_path, strerror(errno));
            closedir(dirp);
            return -1;
        }

        if (closedir(dirp) != 0) {
            fprintf(stderr, "Failed to close `dirp`: %s\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}

enum RARY_SYSTEMD_status RARY_SYSTEMD_get_status(struct RARY_SYSTEMD_unit *unit)
{

    char *command = NULL;
    char *buf = NULL;

    unit->status = RARY_SYSTEMD_STATUS_ERROR;
    
    if ((command = RARY_STRING_create("/usr/bin/systemctl is-active %s 2>&1", unit->name, NULL)) == NULL) {
        fprintf(stderr, "Failed to create formatted string\n");
        goto simple_ret;
    }

    if (RARY_UTILS_run_proc(command, &buf) == -1) {
        fprintf(stderr, "Failed to run command '%s': internal error\n", command);
        goto free_command;
    }

    if (strcmp(buf, "active\n") == 0) {
        unit->status = RARY_SYSTEMD_STATUS_ACTIVE;
    } else if (strcmp(buf, "inactive\n") == 0) {
        unit->status = RARY_SYSTEMD_STATUS_INACTIVE;
    } else if (strcmp(buf, "failed\n") == 0) {
        unit->status = RARY_SYSTEMD_STATUS_FAILED;
    } else {
        fprintf(stderr, "Unknown output from 'systemctl is-active': %s\n", buf);
        // Set unit->status again
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
    }

    free(buf);

free_command:
    free(command);

simple_ret:
    return unit->status;
}

enum RARY_SYSTEMD_state RARY_SYSTEMD_get_state(struct RARY_SYSTEMD_unit *unit)
{
    char *command;
    char *buf = NULL;

    unit->state = RARY_SYSTEMD_STATE_ERROR;
    
    if ((command = RARY_STRING_create("/usr/bin/systemctl is-enabled %s 2>&1", unit->name, NULL)) == NULL) {
        fprintf(stderr, "Failed to create formatted string\n");
        goto simple_ret;
    }

    if (RARY_UTILS_run_proc(command, &buf) == -1) {
        fprintf(stderr, "Failed to run command '%s': internal error\n", command);
        goto free_command;
    }

    if (strcmp(buf, "enabled\n") == 0) {
        unit->state = RARY_SYSTEMD_STATE_ENABLED;
    } else if (strcmp(buf, "disabled\n") == 0) {
        unit->state = RARY_SYSTEMD_STATE_DISABLED;
    } else {
        fprintf(stderr, "Unknown output from 'systemctl is-enabled': %s\n", buf);
        unit->state = -RARY_SYSTEMD_STATE_ERROR;
    }

    free(buf);

free_command:
    free(command);

simple_ret:
    return unit->state;
}

void RARY_SYSTEMD_free_unit(struct RARY_SYSTEMD_unit *unit)
{

    if (unit == NULL) {
        return;
    }

    if (unit->name != NULL) {
        free(unit->name);
    }

    free(unit);
}

struct RARY_SYSTEMD_unit *RARY_SYSTEMD_get_unit(const char *name)
{

    struct RARY_SYSTEMD_unit *unit = NULL;
    struct RARY_SYSTEMD_unit *result = NULL;

    switch (RARY_SYSTEMD_is_unit_exist(name)) {
    case 1:
        // Unis exist, move forward
        break;
    case 0:
        fprintf(stderr, "Failed to get systemd unit: %s is not found!\n", name);
        goto simple_ret;
    case -1:
        fprintf(stderr, "Failed to get systemd unit: internal error\n");
        goto simple_ret;
    default:
        // Impossible to get here
        fprintf(stderr, "Got an impossiple value from RARY_SYSTEMD_is_unit_exit(). HOW??\n");
        goto simple_ret;
    }

    /*
     * At this point, `name` is assumed as a valid systemd unit
     */

    // Allocate memory for `struct RARY_SYSTEMD_unit`
    if ((unit = calloc(1, sizeof(struct RARY_SYSTEMD_unit))) == NULL) {
        fprintf(stderr, "Failed to allocate memory for struct RARY_SYSTEMD_unit\n");
        goto simple_ret;
    }

    // Duplicate `name`, dont want to rely on `name` pointer later
    if ((unit->name = strdup(name)) == NULL) {
        fprintf(stderr, "Failed to duplicate name: %s\n", strerror(errno));
        goto free_unit;
    }

    // Initialize the `status` with the actual state
    if ((unit->status = RARY_SYSTEMD_get_status(unit)) == RARY_SYSTEMD_STATUS_ERROR) {
        fprintf(stderr, "Failed to get the status of %s: internal error\n", name);
        goto free_unit;
    }

    // Initialize the `state` with the actual state.
    if ((unit->state = RARY_SYSTEMD_get_state(unit)) == RARY_SYSTEMD_STATE_ERROR) {
        fprintf(stderr, "Failed to check if %s is enabled: internal error\n", name);
        goto free_unit;
    }

    result = unit;
    unit = NULL;

free_unit:
    RARY_SYSTEMD_free_unit(unit);

simple_ret:
    return result;
}

int RARY_SYSTEMD_do(struct RARY_SYSTEMD_unit *unit, const char *order)
{
    char *command;

    int ret_code;
    char *buf;

    int result = -1;

    if ((strcmp(order, "start")   != 0) &&
        (strcmp(order, "stop")    != 0) &&
        (strcmp(order, "enable")  != 0) &&
        (strcmp(order, "disable") != 0) &&
        (strcmp(order, "restart") != 0))
    {
        fprintf(stderr, "Invalid order given: %s\n", order);
        goto simple_ret;
    } 

    if ((command = RARY_STRING_create("/usr/bin/systemctl %s %s 2>&1", order, unit->name)) == NULL) {
        fprintf(stderr, "Failed to create formatted string\n");
        goto simple_ret;
    }

    switch ((ret_code = RARY_UTILS_run_proc(command, &buf))) {
    case -1:
        fprintf(stderr, "Failed to %s %s: internal error\n", order, unit->name);
        result = -1;
        break;
    case 0:
        result = 0;
        break;
    default:
        fprintf(stderr, "Failed to %s %s: return code of 'systemctl %s': %d\n",
                        order,
                        unit->name,
                        order,
                        ret_code);
        fprintf(stderr, "Output: %s\n", buf);
        result = 1;
        break;
    }

    free(command);
    free(buf);

simple_ret:
    return result;
}

enum RARY_SYSTEMD_status RARY_SYSTEMD_start(struct RARY_SYSTEMD_unit *unit)
{
    int ret_code;

    switch ((ret_code = RARY_SYSTEMD_do(unit, "start"))) {
    case -1:
        fprintf(stderr, "Failed to start %s: internal error\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    case 0:
        unit->status = RARY_SYSTEMD_STATUS_ACTIVE;
        break;
    case 1:
        fprintf(stderr, "Failed to start %s\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    default:
        fprintf(stderr, "Failed to start %s: unknown return code: %d\n", unit->name, ret_code);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    }

    return unit->status;
}

enum RARY_SYSTEMD_status RARY_SYSTEMD_stop(struct RARY_SYSTEMD_unit *unit)
{
    int ret_code;

    switch ((ret_code = RARY_SYSTEMD_do(unit, "stop"))) {
    case -1:
        fprintf(stderr, "Failed to stop %s: internal error\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    case 0:
        unit->status = RARY_SYSTEMD_STATUS_INACTIVE;
        break;
    case 1:
        fprintf(stderr, "Failed to stop %s\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    default:
        fprintf(stderr, "Failed to stop %s: unknown return code: %d\n", unit->name, ret_code);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    }

    return unit->status;
}

enum RARY_SYSTEMD_status RARY_SYSTEMD_restart(struct RARY_SYSTEMD_unit *unit)
{
    int ret_code;

    switch ((ret_code = RARY_SYSTEMD_do(unit, "restart"))) {
    case -1:
        fprintf(stderr, "Failed to restart %s: internal error\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    case 0:
        unit->status = RARY_SYSTEMD_STATUS_ACTIVE;
        break;
    case 1:
        fprintf(stderr, "Failed to restart %s\n", unit->name);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    default:
        fprintf(stderr, "Failed to restart %s: unknown return code: %d\n", unit->name, ret_code);
        unit->status = RARY_SYSTEMD_STATUS_ERROR;
        break;
    }

    return unit->status;
}

enum RARY_SYSTEMD_state RARY_SYSTEMD_enable(struct RARY_SYSTEMD_unit *unit)
{

    int ret_code;

    switch ((ret_code = RARY_SYSTEMD_do(unit, "enable"))) {
    case -1:
        fprintf(stderr, "Failed to enable %s: internal error\n", unit->name);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    case 0:
        unit->state = RARY_SYSTEMD_STATE_ENABLED;
        break;
    case 1:
        fprintf(stderr, "Failed to enable %s\n", unit->name);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    default:
        fprintf(stderr, "Failed to enable %s: unknown return code: %d\n", unit->name, ret_code);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    }

    return unit->state;
}

enum RARY_SYSTEMD_state RARY_SYSTEMD_disable(struct RARY_SYSTEMD_unit *unit)
{

    int ret_code;

    switch ((ret_code = RARY_SYSTEMD_do(unit, "disable"))) {
    case -1:
        fprintf(stderr, "Failed to disable %s: internal error\n", unit->name);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    case 0:
        unit->state = RARY_SYSTEMD_STATE_DISABLED;
        break;
    case 1:
        fprintf(stderr, "Failed to disable %s\n", unit->name);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    default:
        fprintf(stderr, "Failed to disable %s: unknown return code: %d\n", unit->name, ret_code);
        unit->state = RARY_SYSTEMD_STATE_ERROR;
        break;
    }

    return unit->state;
}

char *RARY_SYSTEMD_strstatus(enum RARY_SYSTEMD_status status)
{

    switch (status) {
    case RARY_SYSTEMD_STATUS_ACTIVE:    return "active (running)";
    case RARY_SYSTEMD_STATUS_INACTIVE:  return "inactive (dead)";
    case RARY_SYSTEMD_STATUS_FAILED:    return "failed";
    case RARY_SYSTEMD_STATUS_ERROR:     return "error";
    default:                            return "unknown (?)";
    }
}

char *RARY_SYSTEMD_strstate(enum RARY_SYSTEMD_state state)
{

    switch (state) {
    case RARY_SYSTEMD_STATE_ENABLED:    return "enabled";
    case RARY_SYSTEMD_STATE_DISABLED:   return "disabled";
    case RARY_SYSTEMD_STATE_ERROR:      return "error";
    default:                            return "unknown (?)";
    }
}
