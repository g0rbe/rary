/*
 * file.c example and test
 * 
 * Build:
 *      gcc file_example.c ../src/file.c
 */


#include "../src/rary.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc != 4) {
        printf("Usage ./a.out <src-file> <dst-file> <user>\n");
        return 1;
    }

    if (RARY_FILE_copy(argv[1], argv[2], 0) != 0) {
        fprintf(stderr, "Failed to copy %s\n", argv[1]);
        exit(1);
    } else {
        printf("Successfully copied %s\n", argv[1]);
    }

    if (RARY_FILE_chown(argv[2], argv[3]) != RARY_OK) {
        fprintf(stderr, "Failed to change owner of %s\n", argv[2]);
        exit(1);
    } else {
        printf("Owner changed!\n");
    }
}