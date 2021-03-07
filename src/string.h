#ifndef RARY_STRING_H
#define RARY_STRING_H

#include <stdio.h>

char *RARY_STRING_create(const char *format, ...);

char **RARY_STRING_split(const char *str, const char *delim);

void RARY_STRING_array_free(char **array, size_t size);

char *RARY_STRING_read(FILE* file);

int RARY_STRING_write(const char *file, const char *message, const char *mode);

#endif