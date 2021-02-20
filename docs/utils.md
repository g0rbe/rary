- [Functions](#functions)
  * [`RARY_get_fqdn`](#-rary-get-fqdn-)
  * [`RARY_read_to_string`](#-rary-read-to-string-)
  * [`RARY_run_proc`](#-rary-run-proc-)
  * [`RARY_write_to_file`](#-rary-write-to-file-)
  * [`G_string_array_free`](#-g-string-array-free-)
  * [`RARY_string_split`](#-rary-string-split-)


# Functions

## `RARY_get_fqdn`

```c
char *RARY_get_fqdn(void)
```

**DESCRIPTION**

Returns the fully qualified domain of the computer.
Equal to `hostname -f`.

**RETURN**

- Success: A pointer to a dynamically allocated string containing the fqdn.
- Error: `NULL`

## `RARY_read_to_string`

```c
char *G_read_to_string(FILE* file)
```

**DESCRIPTION**

Read the content of `file` into a dynamically allocated string.

**RETURN**

- Success: A pointer to a dynamically allocated string of `file`'s content.
- Error: `NULL`

## `RARY_run_proc`

```c
int G_run_proc(const char *command, char **buf)
```

**DESCRIPTION**

Runs `command` and store the result in `buf`.
The `buf` must **not** be intialized, the pointer will be replaced by `calloc()`.


If `buf` is `NULL` (not set), then `RARY_run_proc` is not try to read the `stdout`, equal to `system("command > /dev/null")`.
This is usefull if you need the return code, but dont care about the outout.

In case of internal error, the `buf` will be `NULL`.
  
**RETURN**

- `>= 0`: the command's return value.
- `-1`: internal error.

## `RARY_write_to_file`

```c
int G_write_to_file(const char *filename, const char *message, const char *mode)
```

**DESCRIPTION**

Write `message` to `file`.
Mode should be `"w"` to truncate the file, or `"a"` to append to the file (same as `fopen()`).

**RETURN**

 - Success: 0
 - Fail: -1

## `G_string_array_free`

```c
void G_string_array_free(char **array, size_t size)
```

**DESCRIPTION**

Frees every element of the array and the array itself.

If `size` >= 0, then it frees the first `size`-1 element.

If `size` < 0, frees the elements until a `NULL` found, so the array must be `NULL` terminated.

## `RARY_string_split`

```c
char **G_string_split(const char *str, const char *delim)
```

**DESCRIPTION**

Split `str` with `delim` into an array.

The original string is left untouched.

The result must be freed with `G_string_array_free`.

**RETURN**

 - Success: Returns an array of strings, terminated with a `NULL`.
 - Error: `NULL`.