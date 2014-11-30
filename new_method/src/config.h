#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

#include "defs.h"

#define NUM_OF_VALID_KEYS (9)


/* Output type */
typedef enum {
    OUTPUT_NORMAL,
    OUTPUT_JSON
} output_t;

extern char valid_keys[NUM_OF_VALID_KEYS][16];

/* Public Functions */

output_t parse_arguments(int argc,
                         char ** argv,
                         struct location * loc,
                         struct tm * date);

int load_config_from_file(const char * config_filename,
                          struct location * loc);

#endif
