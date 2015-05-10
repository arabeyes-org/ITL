#include "config.h"

/* List of valid keys in the config file */
char valid_keys[NUM_OF_VALID_KEYS][16] = {
    "name",
    "latitude",
    "longitude",
    "altitude",
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

    while (isspace((unsigned char) *str)) str++;
    if (*str == '\0') return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

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
    } else if (strcmp(key, valid_keys[3]) == 0) { /* altitude */
        loc->altitude = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[4]) == 0) { /* asr_method */
        loc->asr_method = (asr_method_t)(strtol(value, &save_ptr, 10));
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[5]) == 0) { /* calc_method */
        method_id = (unsigned int)(strtol(value, &save_ptr, 10));
        loc->calc_method = calc_methods[method_id];
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[6]) == 0) { /* extr_method */
        loc->extr_method = (extr_method_t)(strtol(value, &save_ptr, 10));
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[7]) == 0) { /* timezone */
        loc->timezone = strtod(value, &save_ptr);
        if (value == save_ptr) goto ERR;
    } else if (strcmp(key, valid_keys[8]) == 0) { /* daylight */
        loc->daylight = (int)(strtol(value, &save_ptr, 10));
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
    loc->altitude = 5;
    loc->asr_method = SHAFII;
    loc->calc_method = calc_methods[MWL];
    loc->extr_method = NONE;
    loc->timezone = 1;
    loc->daylight = 0;
}


static void print_usage(const char * prog)
{
    fprintf(stderr, "%s [-d YYYY-MM-DD] "
                    "[-f config_filename] [-j] [-h]\n\n", prog);
    fprintf(stderr, "where:\n");
    fprintf(stderr, " * YYYY-MM-DD: is the date at which"
                    " you want to compute the prayer times\n");
    fprintf(stderr, " * config_filename: is the path to "
                    "configuration file\n");
    fprintf(stderr, " * -j: make the program produce output in "
                    "JSON format instead of normal text\n");
    fprintf(stderr, " * -h: print this help message\n");
}


int load_config_from_file(const char * config_filename,
                          struct location * loc)
{
    FILE * fp = NULL;
    const char * delimiter = ":";
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
        key = strtok(line, delimiter);
        if (key == NULL) goto LD_ERR;
        key = trim_whitespace(key);
        value = strtok(NULL, delimiter);
        if (value == NULL) goto LD_ERR;
        value = trim_whitespace(value);
        r = add_key_value(key, value, loc);
        if (r != EXIT_SUCCESS) goto LD_ERR;
    }
    fclose(fp);
    return EXIT_SUCCESS;
LD_ERR:
    fprintf(stderr, "Error parsing the config file\n");
    fclose(fp);
    exit(EXIT_FAILURE);
}


output_t parse_arguments(int argc,
                         char ** argv,
                         struct location * loc,
                         struct tm * date)
{
    time_t t;
    int rsp, i;
    struct tm * d = NULL;
    int yyyy = 0, mm = 0, dd = 0;
    unsigned int date_set = 0;
    unsigned int config_from_file = 0;
    output_t output = OUTPUT_NORMAL;

    assert(argc >= 1);
    assert(argv != NULL);
    assert(loc != NULL);
    assert(date != NULL);

    /* make sure that date is cleared */
    memset(date, 0, sizeof(struct tm));

    /* getopt is not portable...
     * Therefore, we do things manually :-(
     */
    i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-j") == 0) {
            output = OUTPUT_JSON;
            i++;
        } else if (strcmp(argv[i], "-d") == 0) {
            i++;
            /* make sure that the next token is a date */
            rsp = sscanf(argv[i], "%4u-%2u-%2u", &yyyy, &mm, &dd);
            if  ((rsp != 3) ||
                 (yyyy < 2000 || yyyy > 2199) ||
                 (mm < 1 || mm > 12) ||
                 (dd < 1 || dd > 31)
                ) {
                fprintf(stderr, "Error! invalid date. ");
                fprintf(stderr, "Date format is:\n");
                fprintf(stderr, " - year must be in [2000,2199]\n");
                fprintf(stderr, " - month must be in [1,12]\n");
                fprintf(stderr, " - day must be in [1,31]\n");
                exit(EXIT_FAILURE);
            }
            date->tm_year = yyyy - 1900;
            date->tm_mon = mm - 1;
            date->tm_mday = dd;
            date_set = 1;
            i++;
        } else if (strcmp(argv[i], "-f") == 0) {
            i++;
            /* make sure we have a file */
            rsp = load_config_from_file(argv[i], loc);
            if (rsp != EXIT_SUCCESS) {
                fprintf(stderr, "Invalid argument to option -f\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            config_from_file = 1;
            i++;
        } else if (strcmp(argv[i], "-h") == 0) {
            i++;
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Invalid option\n");
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* By this point, command line parsing should have
     * been done successfully... */
    if (date_set == 0) {
        time(&t);
        /* TODO: localtime is not thread safe, however,
         * it is the only portable one... */
        d = localtime(&t);
        assert(d != NULL);
        memcpy(date, d, sizeof(struct tm));
    }

    if (config_from_file == 0) {
        set_default_location(loc);
    }

    return output;
}
