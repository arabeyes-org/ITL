/* Needed to expose POSIX stuff under C89 mode */
#define _POSIX_C_SOURCE 2

#include "config.h"

/* List of valid keys in the config file */
char valid_keys[NUM_OF_VALID_KEYS][16] = {
    "name",
    "latitude",
    "longitude",
    "height",
    "asr_method",
    "calc_method",
    "extr_method",
    "timezone",
    "daylight"
};

extern calc_method_t calc_methods [];

static void set_default_location(struct location *loc);

static char * trim_whitespace(char * str);

static int add_key_value(const char * key,
                         const char * value,
                         struct location * loc);


/*
 * Taken from: http://stackoverflow.com/a/122721
 */
static char * trim_whitespace(char * str)
{
    char * end;

    assert(str != NULL);

    while (isspace(*str)) str++;
    if (*str == '\0') return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    /* Write new null terminator */
    *(end + 1) = '\0';
    return str;
}


static int add_key_value(const char * key,
                         const char * value,
                         struct location * loc)
{
    char * save_ptr;
    unsigned int method_id;

    assert(key != NULL);
    assert(value != NULL);
    assert(loc != NULL);

    if (strcmp(key, valid_keys[0]) == 0) { /* name */
        strcpy(loc->name, value);
    } else if (strcmp(key, valid_keys[1]) == 0) { /* latitude */
        loc->latitude = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[2]) == 0) { /* longitude */
        loc->longitude = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[3]) == 0) { /* height */
        loc->height = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[4]) == 0) { /* asr_method */
        loc->asr_method = strtol(value, &save_ptr, 10);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[5]) == 0) { /* calc_method */
        method_id = strtol(value, &save_ptr, 10);
        loc->calc_method = calc_methods[method_id];
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[6]) == 0) { /* extr_method */
        loc->extr_method = strtol(value, &save_ptr, 10);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[7]) == 0) { /* timezone */
        loc->timezone = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[8]) == 0) { /* daylight */
        loc->daylight = strtol(value, &save_ptr, 10);
        if (value == save_ptr) goto ERR;
    } else {
        fprintf(stderr, "Invalid key detected in the config file\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
ERR:
    fprintf(stderr, "Error parsing the value\n");
    return EXIT_FAILURE;
}


static void set_default_location(struct location *loc)
{
    assert(loc != NULL);

    /* The following is for testing purposes only */
    strcpy(loc->name, "Eindhoven, Netherlands");
    loc->latitude = 51.408311;
    loc->longitude = 5.454939;
    loc->height = 5;
    loc->asr_method = SHAFII;
    loc->calc_method = calc_methods[MWL];
    loc->extr_method = NONE;
    loc->timezone = 1;
    loc->daylight = 0;
}


void parse_arguments(int argc,
                     char ** argv,
                     struct location * loc)
{
    assert(argc >= 1 && argc < 3);
    assert(argv != NULL);
    assert(loc != NULL);

    if (argc == 1) {
        fprintf(stdout, "Using default settings...\n");
        set_default_location(loc);
    } else if (argc == 2) {
        load_config_from_file(argv[1], loc);
    }
}


int load_config_from_file(const char * config_filename,
                          struct location * loc)
{
    FILE * fp = NULL;
    const char * delimiter = ":";
    char * saveptr;
    char * key;
    char * value;
    char line[1024];
    int r;

    assert(config_filename != NULL);
    assert(loc != NULL);

    fp = fopen(config_filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "errno %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        key = strtok_r(line, delimiter, &saveptr);
        key = trim_whitespace(key);
        value = strtok_r(NULL, delimiter, &saveptr);
        value = trim_whitespace(value);
        r = add_key_value(key, value, loc);
        if (r != EXIT_SUCCESS) {
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
