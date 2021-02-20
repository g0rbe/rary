- [Terms](#terms)
- [Types](#types)
  * [`enum RARY_SYSTEMD_status`](#-enum-rary-systemd-status-)
  * [`enum RARY_SYSTEMD_state`](#-enum-rary-systemd-state-)
  * [`struct RARY_SYSTEMD_unit`](#-struct-rary-systemd-unit-)
- [Variables](#variables)
  * [`RARY_SYSTEMD_unit_dirs`](#-rary-systemd-unit-dirs-)
- [Functions](#functions)
  * [`RARY_SYSTEMD_is_unit_exist`](#-rary-systemd-is-unit-exist-)
  * [`RARY_SYSTEMD_get_status`](#-rary-systemd-get-status-)
  * [`RARY_SYSTEMD_get_state`](#-rary-systemd-get-state-)
  * [`RARY_SYSTEMD_free_unit`](#-rary-systemd-free-unit-)
  * [`RARY_SYSTEMD_get_unit`](#-rary-systemd-get-unit-)
  * [`RARY_SYSTEMD_do`](#-rary-systemd-do-)
  * [`RARY_SYSTEMD_start`](#-rary-systemd-start-)
  * [`RARY_SYSTEMD_stop`](#-rary-systemd-stop-)
  * [`RARY_SYSTEMD_enable`](#-rary-systemd-enable-)
  * [`RARY_SYSTEMD_disable`](#-rary-systemd-disable-)
  * [`RARY_SYSTEMD_strstatus`](#-rary-systemd-strstatus-)
  * [`RARY_SYSTEMD_strstate`](#-rary-systemd-strstate-)


# Terms

- Status: The unit is "active (running)", "inactive (dead) or "failed"
    - Get with `systemctl is-active`.
- State: The unit is "enabled" or "disabled" to start automatically.
    - Get with `systemctl is-enabled`.

# Types

## `enum RARY_SYSTEMD_status`

```c
enum RARY_SYSTEMD_status {
    RARY_SYSTEMD_STATUS_INACTIVE = 1,
    RARY_SYSTEMD_STATUS_ACTIVE = 2,
    RARY_SYSTEMD_STATUS_FAILED = 3,
    RARY_SYSTEMD_STATUS_ERROR = -1
};
```

- `RARY_SYSTEMD_STATUS_INACTIVE` = Unit is not running.
- `RARY_SYSTEMD_STATUS_ACTIVE` = Unit is running.
- `RARY_SYSTEMD_STATUS_FAILED` = Unit is failed.
- `RARY_SYSTEMD_STATUS_ERROR` = Failed to get status, indicate an error.

## `enum RARY_SYSTEMD_state`

```c
enum RARY_SYSTEMD_state {
    RARY_SYSTEMD_STATE_ENABLED = 1,
    RARY_SYSTEMD_STATE_DISABLED = 2,
    RARY_SYSTEMD_STATE_ERROR = -1
};
```
- `RARY_SYSTEMD_STATE_ENABLED`: The unit is enabled.
- `RARY_SYSTEMD_STATE_DISABLED`: The unit is disabled.
- `RARY_SYSTEMD_STATE_ERROR`: Failed to get status, indicate an error.

## `struct RARY_SYSTEMD_unit`

```c
struct RARY_SYSTEMD_unit {
    char *name;
    enum RARY_SYSTEMD_status status;
    enum RARY_SYSTEMD_state state;
};
```

- `name`: Name of the service.
- `status`: Unit status as defined in `RARY_SYSTEMD_status`.
- `state`: Unit state defined in `RARY_SYSTEMD_state`.

# Variables

## `RARY_SYSTEMD_unit_dirs`

```c
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
```

The list of direcories to search for the unit names.
Terminated with a `NULL`, to know when to stop.

# Functions

## `RARY_SYSTEMD_is_unit_exist`

```c
int RARY_SYSTEMD_is_unit_exist(const char *name);
```

**DESCRIPTION**

Check if the given unit name is exist in one of the preconfigured directory.
The list is set in `RARY_SYSTEMD_unit_dirs`.

**RETURN**

- `1`: the unit is found.
- `0`: the unit is **not** found.
- `-1`: internal error.

## `RARY_SYSTEMD_get_status`

```c
enum RARY_SYSTEMD_status RARY_SYSTEMD_get_status(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Get the status of the given unit.


The returned value is the same as in `unit->status`.
(Actually, the `unit->status` is returned)

**RETURN**

- `enum RARY_SYSTEMD_status`.
- `RARY_SYSTEMD_ERROR` is indicate an internal error.

## `RARY_SYSTEMD_get_state`

```c
enum RARY_SYSTEMD_state RARY_SYSTEMD_get_state(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Check wheter the given unit is enabled or not.

The returned value is the same as in `unit->status`.
(Actually, the `unit->status` is returned)

**RETURN**

- `enum RARY_SYSTEMD_state`.
- `RARY_SYSTEMD_STATE_ERROR` indicates an error.

## `RARY_SYSTEMD_free_unit`

```c
void RARY_SYSTEMD_free_unit(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Free the allocated memry of the given unit.

## `RARY_SYSTEMD_get_unit`

```c
struct RARY_SYSTEMD_unit *RARY_SYSTEMD_get_unit(const char *name);
```

**DESCRIPTION**

Create a unit struct of the given unit with `name`.


This should be the first function from `systemd.h` to start use the lib*rary*.


This function set the `status` and `state` parameters.
But don't rely on it too long, these values may be changed in the unit by other processes.


Always use `RARY_SYSTEMD_free_unit` to free the unit.

**RETURN**

- Success: a pointer to dynamically allocated `struct RARY_SYSTEMD_unit`.
- Fail: `NULL`

## `RARY_SYSTEMD_do`

```c
int RARY_SYSTEMD_do(struct RARY_SYSTEMD_unit *unit, const char *order);
```

**DESCRIPTION**

Start, stop, enable or disable the given unit.


This function is not set the underlying `status` or `state`.

Use `RARY_SYSTEMD_start`, `RARY_SYSTEMD_stop`, `RARY_SYSTEMD_enable` or `RARY_SYSTEMD_disable` instead.

**RETURN**

- `0`: Order successful on the given unit.
- `1`: Failed to do the order.
- `-1`: Internal error.

## `RARY_SYSTEMD_start`

```c
enum RARY_SYSTEMD_status RARY_SYSTEMD_start(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Start the given unit.


The returned value will be the same as `unit->status`.

**RETURN**

- `RARY_SYSTEMD_STATUS_ACTIVE`: Unit started successfully.
- `RARY_SYSTEMD_STATUS_ERROR`: Failed to start.

## `RARY_SYSTEMD_stop`

```c
enum RARY_SYSTEMD_status RARY_SYSTEMD_stop(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Stop the given unit.


The returned value will be the same as `unit->status`.

**RETURN**

- `RARY_SYSTEMD_STATUS_INACTIVE`: Unit is stopped successfully.
- `RARY_SYSTEMD_STATUS_ERROR`: An error occured.

## `RARY_SYSTEMD_enable`

```c
enum RARY_SYSTEMD_state RARY_SYSTEMD_enable(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Enable the given unit.


The returned value will be the same as `unit->state`.

**RETURN**

- `RARY_SYSTEMD_STATE_ENABLED`: Unit is enabled successfully.
- `RARY_SYSTEMD_STATE_ERROR`: An error occured.

## `RARY_SYSTEMD_disable`

```c
enum RARY_SYSTEMD_state RARY_SYSTEMD_disable(struct RARY_SYSTEMD_unit *unit);
```

**DESCRIPTION**

Disable the given unit.


The returned value will be the same as `unit->state`.

**RETURN**

- `RARY_SYSTEMD_STATE_DISABLED`: Unit is disabled successfully.
- `RARY_SYSTEMD_STATE_ERROR`: An error occured.

## `RARY_SYSTEMD_strstatus`

```c
char *RARY_SYSTEMD_strstatus(enum RARY_SYSTEMD_status status);
```

**DESCRIPTION**

Returns a string describing the `status`. 

## `RARY_SYSTEMD_strstate`

```c
char *RARY_SYSTEMD_strstate(enum RARY_SYSTEMD_state state);
```

**DESCRIPTION**

Returns a string describing the `state`.