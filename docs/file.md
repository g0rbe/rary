# Functions

## RARY_FILE_copy

```c
int RARY_FILE_copy(const char *source, const char *dest, int flag);
```
**DESCRIPTION**

Copy the content of `source` to `dest`.

The function will overwite the `dest` if it is exist.

`flag`:

- `RARY_FILE_NOVERWRITE`: Do not overwrite the destination file, returns `-1` if `dest` is exist.

**RETURN**

- `RARY_OK`: Success
- `RARY_FAIL`: Failed

## `RARY_FILE_chown_str`

```c
int RARY_FILE_chown(const char *path, const char *name);
```

**DESCRIPTION**

Change the owner and the group of `path`.

It set the owner to `name`.

**RETURN**

- `RARY_OK`: Success
- `RARY_FAIL`: Failed