#ifndef RARY_FILE_H
#define RARY_FILE_H

#define RARY_FILE_OVERWRITE 1
#define RARY_FILE_NOVERWRITE 2

int RARY_FILE_copy(const char *source, const char *dest, int flag);

int RARY_FILE_chown(const char *path, const char *name);

#endif