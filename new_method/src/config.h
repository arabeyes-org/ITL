#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

#include "defs.h"

#define NUM_OF_VALID_KEYS (9)

/* Public Functions */

void parse_arguments(int argc,
                     char ** argv,
                     struct location * loc);

int load_config_from_file(const char * config_filename,
                          struct location * loc);

#endif
