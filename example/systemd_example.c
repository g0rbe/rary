/*
 * Rary - systemd example
 * 
 * Build:
 *      gcc systemd_example.c ../src/systemd.c ../src/utils.c ../src/string.c
 */

#include "../src/rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

    if (geteuid() != 0) {
        fprintf(stderr, "Run as root!\n");
        exit(1);
    }

    struct RARY_SYSTEMD_unit *unit;

    enum RARY_SYSTEMD_status init_status;
    enum RARY_SYSTEMD_state init_state;

    /*
     * Create a new unit.
     */
    if ((unit = RARY_SYSTEMD_get_unit("cups.service")) == NULL) {
        fprintf(stderr, "Failed to get systemd unit 'cups.service'\n");
        exit(1);
    }

    // Save the initial state to restore it later
    init_status = unit->status;
    init_state = unit->state;

    // Just print the infos
    printf("Unit name: %s\n", unit->name);
    printf("Unit status: %s\n", RARY_SYSTEMD_strstatus(unit->status));
    printf("Unit is enabled: %s\n", RARY_SYSTEMD_strstate(unit->state));

    // The unit is failed, need to check why
    if (unit->status == RARY_SYSTEMD_STATUS_FAILED) {
        printf("seems like %s is failed, exiting...\n", unit->name);
        RARY_SYSTEMD_free_unit(unit);
        exit(1);
    }

    /*
     * Stop the unit if active
     */
    if (unit->status == RARY_SYSTEMD_STATUS_ACTIVE) {
        printf("Stopping %s...\n", unit->name);
        
        if (RARY_SYSTEMD_stop(unit) != RARY_SYSTEMD_STATUS_INACTIVE) {
            fprintf(stderr, "Failed to stop %s\n", unit->name);
            RARY_SYSTEMD_free_unit(unit);
            exit(1);
        }
    }
   
    sleep(3);

    /*
     * Start the unit
     */
    printf("Starting %s...\n", unit->name);

    if (RARY_SYSTEMD_start(unit) != RARY_SYSTEMD_STATUS_ACTIVE) {
        fprintf(stderr, "Failed to start %s\n", unit->name);
        RARY_SYSTEMD_free_unit(unit);
        exit(1);
    }

    sleep(3);

    /*
     * Enable the unit
     */
    if (unit->state == RARY_SYSTEMD_STATE_DISABLED) {
        printf("Seems like %s is disabled, enabling...\n", unit->name);

        if (RARY_SYSTEMD_enable(unit) != RARY_SYSTEMD_STATE_ENABLED) {
            fprintf(stderr, "Failed to enable %s\n", unit->name);
            RARY_SYSTEMD_free_unit(unit);
            exit(1);
        }
    }

    sleep(3);

    /*
     * Disable the unit.
     */

    printf("Disabling %s...\n", unit->name); 

    if (RARY_SYSTEMD_disable(unit) != RARY_SYSTEMD_STATE_DISABLED) {
        fprintf(stderr, "Failed to disable %s\n", unit->name);
        RARY_SYSTEMD_free_unit(unit);
        exit(1);
    }

    sleep(3);

    /*
     * Restart the unit
     */

    printf("Restaring %s...\n", unit->name);

    if (RARY_SYSTEMD_restart(unit) != RARY_SYSTEMD_STATUS_ACTIVE) {
        fprintf(stderr, "Failed to restart %s\n", unit->name);
        RARY_SYSTEMD_free_unit(unit);
        exit(1);
    }

    sleep(3);

    /*
     * Restore to the original state
     */
    if (init_status == RARY_SYSTEMD_STATUS_ACTIVE) {

        printf("Starting %s, because it was stared before the example\n", unit->name);

        if (RARY_SYSTEMD_start(unit) != RARY_SYSTEMD_STATUS_ACTIVE) {
            fprintf(stderr, "Failed to start %s\n", unit->name);
        }
    }

    if (init_state == RARY_SYSTEMD_STATE_ENABLED) {

        printf("Enabling %s, because it was enabled before the example\n", unit->name);

        if (RARY_SYSTEMD_enable(unit) != RARY_SYSTEMD_STATE_ENABLED) {
            fprintf(stderr, "Failed to enable %s\n", unit->name);
        }
    }

    RARY_SYSTEMD_free_unit(unit);
}