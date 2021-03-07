/*
 * RARY_STRING_create example (and test)
 * 
 * Build:
 *      gcc ../src/string.c string_example.c
 * 
 */

#include "../src/rary.h"

#include <stdio.h>
#include <stdlib.h>

void RARY_STRING_create_test(void)
{
    char *result = RARY_STRING_create("The %s is blue, the %s is green", "sky", "grass");

    if (result == NULL) {
        fprintf(stderr, "Failed to create string!\n");
        exit(1);
    }

    printf("%s\n", result);
    free(result);
}

void RARY_STRRARY_STRING_split_test(void)
{
    char *to_split = calloc(20, sizeof(char));

    if (to_split == NULL) {
        fprintf(stderr, "Failed to allocate memory for to_split\n");
        exit(1);
    }

    snprintf(to_split, 19, "a\nb\nc\nd\n");

    char **res = RARY_STRING_split(to_split, "\n");


    if (res == NULL) {
        fprintf(stderr, "Failed to split test string\n");
        exit(1);
    }

    printf("original:\n%s\n", to_split); 

    printf("result:\n");
    for (char **elem = res; *elem != NULL; elem++) {
        printf("%s\n", *elem);
    }

    free(to_split);
    RARY_STRING_array_free(res, -1);
}

int main(void)
{
    RARY_STRING_create_test();

    RARY_STRRARY_STRING_split_test();

}