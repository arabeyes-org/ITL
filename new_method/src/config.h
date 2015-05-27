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

#define KEY_NAME_LENGTH   (16)

/**
  * Output type
  */
typedef enum {
    OUTPUT_NORMAL, /**< Textual output */
    OUTPUT_JSON    /**< JSON output */
} output_t;

/**
  * key names
  */
struct key_names {
    char name[KEY_NAME_LENGTH];
    char latitude[KEY_NAME_LENGTH];
    char longitude[KEY_NAME_LENGTH];
    char altitude[KEY_NAME_LENGTH];
    char asr_method[KEY_NAME_LENGTH];
    char calc_method[KEY_NAME_LENGTH];
    char extr_method[KEY_NAME_LENGTH];
    char timezone[KEY_NAME_LENGTH];
    char daylight[KEY_NAME_LENGTH];
};

extern struct key_names valid_keys;

/* Public Functions */

int parse_arguments(int argc,
                    char ** argv,
                    struct location * loc,
                    struct tm * date,
                    output_t * output);

int load_config_from_file(const char * config_filename,
                          struct location * loc);

#endif
