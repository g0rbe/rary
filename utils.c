#include "utils.h"
#include "rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

char *RARY_get_fqdn(void) {

    char *hostname;
    struct hostent* h;

    if ((hostname = calloc(512, sizeof(char))) == NULL) {
        fprintf(stderr, "Failed to allocate memory for hostname\n");
        return NULL;
    }

    if (gethostname(hostname, 511) != 0) {
        fprintf(stderr, "Failed to get hostname: %s\n", strerror(errno));
        free(hostname);
        return NULL;
    }

    if ((h = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "Failed to get host by name: %d\n", h_errno);
        free(hostname);
        return NULL;
    }

    memset(hostname, 0, 512);
    strncpy(hostname, h->h_name, 511);
    
    return hostname;
}

char *RARY_read_to_string(FILE* file) {

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

int RARY_run_proc(const char *command, char *buff) {
	
	FILE *proc = NULL;
    int res;

	if ((proc = popen(command, "r")) == NULL) {
		fprintf(stderr, "Failed to open process '%s': %s\n", command, strerror(errno));
		return -1;
	}
	
    if ((buff = RARY_read_to_string(proc)) == NULL) {
        fprintf(stderr, "Failed to read from process\n");
        return -1;
    }
	
	if ((res = pclose(proc)) == -1) {
		free(buff);
		buff = NULL;
		return -1;
	}
	
	return res;	
}

int RARY_write_to_file(const char *filename, const char *message, const char *mode) {

	FILE *file = NULL;
    int err;

	if ((strcmp(mode, "w") != 0) || (strcmp(mode, "a") != 0)) {
		fprintf(stderr, "Invalid mode given: %s (must be \"w\" or \"a\")\n", mode);
		return -1;
	}

	if ((file = fopen(filename, mode)) == NULL) {
		fprintf(stderr, "Failed to open '%s': %s", filename, strerror(errno));
		return -1;
	}
		
	if (fputs(message, file) == EOF) {
		fprintf(stderr, "Failed to write to '%s'", filename);
		fclose(file);
		return -1;
	}

    if ((err = ferror(file)) != 0) {
        fprintf(stderr, "Error found in the file stream: %d\n", err);
        fclose(file);
        return -1;
    }
	
	if (fclose(file) != 0) {
		fprintf(stderr, "Failed to close '%s': %s", filename, strerror(errno));
		return -1;
	}

	return 0;
}

void RARY_string_array_free(char **array, size_t size) {

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

char **RARY_string_split(const char *str, const char *delim) {

    // `dupped' is used to duplicate `str', to prevent to modify `str'.
    // `dupped_free' is used to store the pointer of `dupped' to be able to free it later. 
    char *dupped, *dupped_free;
    char *tok;
    char **str_arr = NULL;
    int arr_size = 0, arr_index = 0;

    if ((dupped = strdup(str)) == NULL) {
        fprintf(stderr, "Failed to duplicate `str`: %s\n", strerror(errno));
        return NULL;
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
            free(dupped_free);
            return NULL;
        }

        if ((str_arr[arr_index] = strdup(tok)) == NULL) {
            fprintf(stderr, "Failed to duplicate the token\n");
            free(dupped_free);
            RARY_string_array_free(str_arr, arr_size);
            return NULL;
        }

        arr_index++;
    }

    arr_size++;

    // Append a `NULL' to the end of the array.
    if ((str_arr = realloc(str_arr, arr_size * sizeof(char *))) == NULL) {
        fprintf(stderr, "Failed to reallocate memory for str_arr\n");
        free(dupped_free);
        return NULL;
    }

    str_arr[arr_index] = NULL;

    free(dupped_free);

    return str_arr;
}