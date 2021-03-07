#include "rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

char *RARY_STRING_create(const char *format, ...)
{
    char *result = NULL;
    size_t size = 8;
    va_list args;

    /*
     * Inifity loop.
     * Realloc result until teh size is enough for the format and the args. 
     */
    while(1) {

        va_start(args, format);

        if ((result = realloc(result, size * sizeof(char))) == NULL) {
            fprintf(stderr, "Failed to allocate memory for result\n");
            goto end_list;
        }

        if (vsnprintf(result, size, format, args) < size) {
            goto end_list;
        }
#ifdef RARY_DEBUG
        else {
            printf("size %ld was too small\n", size);
        }
#endif

        va_end(args);
        
        size += size;
    }

end_list:
    va_end(args);

    return result;
}

char **RARY_STRING_split(const char *str, const char *delim) {

    // `dupped' is used to duplicate `str', to prevent to modify `str'.
    // `dupped_free' is used to store the pointer of `dupped' to be able to free it later. 
    char *dupped, *dupped_free;
    char *tok;
    char **str_arr = NULL;
    int arr_size = 0, arr_index = 0;

    if ((dupped = strdup(str)) == NULL) {
        fprintf(stderr, "Failed to duplicate `str`: %s\n", strerror(errno));
        goto simple_ret;
    }

    dupped_free = dupped;

    while ((tok = strsep(&dupped, delim)) != NULL) {

        // Skip empty tokens
        if (strlen(tok) == 0) {
            continue;
        }

        arr_size++;

        if ((str_arr = realloc(str_arr, arr_size * sizeof(char *))) == NULL) {
            fprintf(stderr, "Failed to reallocate memory for str_arr\n");
            goto free_dupped;
        }

        if ((str_arr[arr_index] = strdup(tok)) == NULL) {
            fprintf(stderr, "Failed to duplicate the token\n");
            RARY_STRING_array_free(str_arr, arr_size);
            goto free_dupped;
        }

        arr_index++;
    }

    arr_size++;

    // Append a `NULL' to the end of the array.
    if ((str_arr = realloc(str_arr, arr_size * sizeof(char *))) == NULL) {
        fprintf(stderr, "Failed to reallocate memory for str_arr\n");
        goto free_dupped;
    }

    str_arr[arr_index] = NULL;

free_dupped:
    free(dupped_free);

simple_ret:
    return str_arr;
}

void RARY_STRING_array_free(char **array, size_t size)
{

    if (array == NULL) {
        return;
    }

    /*
     * If `size' specified, use it as a counter.
     * Otherwise, the array should end with `NULL'.
     */
    if (size < 0) {
        for (int i = 0; i < size; i++) {
            free(array[i]);
        }
    } else {
        for (char **temp = array; *temp != NULL; temp++) {
            free(*temp);
        }
    }

    free(array);
}

char *RARY_STRING_read(FILE* file)
{

    char *str;
    char ch;

    //Start size
    size_t size = 128;
    size_t len = 0;

    if((str = calloc(size, sizeof(char))) == NULL) {
        fprintf(stderr, "Failed to allocate memory for str\n");
        return NULL;
    }

    while((ch = fgetc(file)) != EOF){
        
        if(len >= size-1){

            // Increase with a 512 byte block
            size += 128;

            if((str = realloc(str, size * sizeof(char))) == NULL) {
                fprintf(stderr, "Failed to reallocate memory for str\n");
                return NULL;
            }
        }

        str[len++] = ch;
    }

    // Append a terminating '\0' character
    str[len++] = '\0';

    return str;
}

int RARY_STRING_write(const char *file, const char *message, const char *mode)
{

	FILE *filep = NULL;
    int err;

	if ((strcmp(mode, "w") != 0) || (strcmp(mode, "a") != 0)) {
		fprintf(stderr, "Invalid mode given: %s (must be \"w\" or \"a\")\n", mode);
		return RARY_FAIL;
	}

	if ((filep = fopen(file, mode)) == NULL) {
		fprintf(stderr, "Failed to open '%s': %s", file, strerror(errno));
		return RARY_FAIL;
	}
		
	if (fputs(message, filep) == EOF) {
		fprintf(stderr, "Failed to write to '%s'", file);
		fclose(filep);
		return RARY_FAIL;
	}

    if ((err = ferror(filep)) != 0) {
        fprintf(stderr, "Error found in the filep stream: %d\n", err);
        fclose(filep);
        return RARY_FAIL;
    }
	
	if (fclose(filep) != 0) {
		fprintf(stderr, "Failed to close '%s': %s", file, strerror(errno));
		return RARY_FAIL;
	}

	return RARY_OK;
}