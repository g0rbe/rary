#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

char *RARY_get_fqdn(void);
char *RARY_read_to_string(FILE* file);
int RARY_run_proc(const char *command, char *buff);
int RARY_write_to_file(const char *filename, const char *message, const char *mode);
void RARY_string_array_free(char **array, size_t size);
char **RARY_string_split(const char *str, const char *delim);

#endif