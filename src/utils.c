#include "rary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

char *RARY_UTILS_get_fqdn(void)
{

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

int RARY_UTILS_run_proc(const char *command, char **buff)
{
	
	FILE *proc = NULL;
    int res;

	if ((proc = popen(command, "r")) == NULL) {
		fprintf(stderr, "Failed to open process '%s': %s\n", command, strerror(errno));
		return -1;
	}
	
    if (buff != NULL) {
        if ((*buff = RARY_STRING_read(proc)) == NULL) {
            fprintf(stderr, "Failed to read from process\n");
            return -1;
        }
    }
	
	if ((res = pclose(proc)) == -1) {
        fprintf(stderr, "Failed to close process '%s': internal error\n", command);
        
        if (buff != NULL) {
		    free(*buff);
		    *buff = NULL;
        }
	}
	
	return res;	
}



