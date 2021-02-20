#ifndef _SYSTEMD_H
#define _SYSTEMD_H

#include <stdio.h>

enum RARY_SYSTEMD_status {
    RARY_SYSTEMD_STATUS_INACTIVE = 1,
    RARY_SYSTEMD_STATUS_ACTIVE = 2,
    RARY_SYSTEMD_STATUS_FAILED = 3,
    RARY_SYSTEMD_STATUS_ERROR = -1
};

enum RARY_SYSTEMD_state {
    RARY_SYSTEMD_STATE_ENABLED = 1,
    RARY_SYSTEMD_STATE_DISABLED = 2,
    RARY_SYSTEMD_STATE_ERROR = -1
};

struct RARY_SYSTEMD_unit {
    char *name;
    enum RARY_SYSTEMD_status status;
    enum RARY_SYSTEMD_state state;
};

int RARY_SYSTEMD_is_unit_exist(const char *name);

enum RARY_SYSTEMD_status RARY_SYSTEMD_get_status(struct RARY_SYSTEMD_unit *unit);

enum RARY_SYSTEMD_state RARY_SYSTEMD_get_state(struct RARY_SYSTEMD_unit *unit);

void RARY_SYSTEMD_free_unit(struct RARY_SYSTEMD_unit *unit);

struct RARY_SYSTEMD_unit *RARY_SYSTEMD_get_unit(const char *name);

int RARY_SYSTEMD_do(struct RARY_SYSTEMD_unit *unit, const char *order);

enum RARY_SYSTEMD_status RARY_SYSTEMD_start(struct RARY_SYSTEMD_unit *unit);

enum RARY_SYSTEMD_status RARY_SYSTEMD_stop(struct RARY_SYSTEMD_unit *unit);

enum RARY_SYSTEMD_state RARY_SYSTEMD_enable(struct RARY_SYSTEMD_unit *unit);

enum RARY_SYSTEMD_state RARY_SYSTEMD_disable(struct RARY_SYSTEMD_unit *unit);

char *RARY_SYSTEMD_strstatus(enum RARY_SYSTEMD_status status);

char *RARY_SYSTEMD_strstate(enum RARY_SYSTEMD_state state);

#endif