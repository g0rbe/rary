#include "rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/sendfile.h>
#include <pwd.h>


int RARY_FILE_copy(const char *source, const char *dest, int flag)
{

    int sfd; // Source file descriptor
    int dfd; // Destination file descriptor

    struct stat sstat; // Source file stat

    int wrote; // How much byte we wrote with `sendfile`

    int result = RARY_FAIL;

    if (flag & RARY_FILE_NOVERWRITE) {
        if (access(dest, F_OK) == 0) {
            fprintf(stderr, "Destination file %s is exist!\n", dest);
            goto simple_ret;
        }
    }

    if ((sfd = open(source, O_RDONLY)) < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", source, strerror(errno));
        goto simple_ret;
    }

    if ((dfd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 00666)) < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", dest, strerror(errno));
        goto close_sfd;
    }

    if (stat(source, &sstat) != 0) {
        fprintf(stderr, "Failed to get the stat of %s: %s\n", source, strerror(errno));
        goto close_dfd;
    }

    // Write up to src size in byte
    wrote = sendfile(dfd, sfd, NULL, sstat.st_size);

    if (wrote == -1) {
        fprintf(stderr, "Failed to copy file: %s\n", strerror(errno));
        goto close_dfd;
    } else if (wrote != sstat.st_size) {
        fprintf(stderr, "Failed to copy file: copied bytes %d/%ld\n", wrote, sstat.st_size);
        goto close_dfd;
    } else {
        result = RARY_OK;
    }

close_dfd:
    close(dfd);

close_sfd:
    close(sfd);

simple_ret:
    return result;
}

int RARY_FILE_chown(const char *path, const char *name)
{

    struct passwd pw;
    struct passwd *temppw;
    char pwbuff[256] = {0};

    if (getpwnam_r(name, &pw, pwbuff, 256, &temppw) != 0) {
        fprintf(stderr, "Failed to get passwd entry of %s: %s\n", name, strerror(errno));
        return RARY_FAIL;
    }

    if (temppw == NULL) {
        fprintf(stderr, "User not found: %s\n", name);
        return RARY_FAIL;
    }

    if (chown(path, pw.pw_uid, pw.pw_gid) != 0) {
        fprintf(stderr, "Failed to change owner of %s: %s\n", path, strerror(errno));
        free(temppw);
        return RARY_FAIL;
    }

    return RARY_OK;
}