#include "config.h"

/* List of valid key names in the config file */
struct key_names valid_keys = {
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


/**
 * Trims the leading and trailing whitespace from a string
 * Taken from: http://stackoverflow.com/a/122721
 * @return Trimmed string
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

/**
 * Adds a new <key,value> pair to the loc struct
 * @return 0 if successful, 1 otherwise
 */
static int add_key_value(const char * key,
                         const char * value,
                         struct location * loc)
{
    char * save_ptr;
    unsigned int method_id;

    assert(key != NULL);
    assert(value != NULL);
    assert(loc != NULL);

    if (strcmp(key, valid_keys.name) == 0) {
        strcpy(loc->name, value);
    } else if (strcmp(key, valid_keys.latitude) == 0) {
        loc->latitude = strtod(value, &save_ptr);
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.longitude) == 0) {
        loc->longitude = strtod(value, &save_ptr);
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.altitude) == 0) {
        loc->altitude = strtod(value, &save_ptr);
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.asr_method) == 0) {
        loc->asr_method = \
          (asr_method_t)(strtol(value, &save_ptr, 10));
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.calc_method) == 0) {
        method_id = (unsigned int)(strtol(value, &save_ptr, 10));
        loc->calc_method = calc_methods[method_id];
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.extr_method) == 0) {
        loc->extr_method = \
          (extr_method_t)(strtol(value, &save_ptr, 10));
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.timezone) == 0) {
        loc->timezone = strtod(value, &save_ptr);
        if (value == save_ptr) return 1;
    } else if (strcmp(key, valid_keys.daylight) == 0) {
        loc->daylight = (int)(strtol(value, &save_ptr, 10));
        if (value == save_ptr) return 1;
    } else {
        return 1;
    }
    return 0;
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
        return 1;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        key = strtok(line, delimiter);
        if (key == NULL) goto LD_ERR;
        key = trim_whitespace(key);
        value = strtok(NULL, delimiter);
        if (value == NULL) goto LD_ERR;
        value = trim_whitespace(value);
        r = add_key_value(key, value, loc);
        if (r != 0) goto LD_ERR;
    }
    r = fclose(fp);
    assert(r == 0);
    return 0;
LD_ERR:
    r = fclose(fp);
    assert(r == 0);
    return 1;
}

/**
  * Parses the command line arguments and constructs the loc
  * data structure out of it
  * @return 0 if all went fine, 1 otherwise
  */
int parse_arguments(int argc,
                    char ** argv,
                    struct location * loc,
                    struct tm * date,
                    output_t * output)
{
    time_t t;
    int rsp, i;
    struct tm * d = NULL;
    int yyyy = 0, mm = 0, dd = 0;
    unsigned int date_set = 0;
    unsigned int config_from_file = 0;
    *output = OUTPUT_NORMAL;

    assert(argc >= 1);
    assert(loc != NULL);
    assert(date != NULL);

    /* make sure that date is cleared */
    memset(date, 0, sizeof(struct tm));

    /* getopt is not portable...
     * Therefore, we do things manually :-( */
    i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-j") == 0) {
            *output = OUTPUT_JSON;
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
                return 1;
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
            if (rsp != 0) {
                fprintf(stderr, "Invalid argument to option -f\n");
                print_usage(argv[0]);
                return 1;
            }
            config_from_file = 1;
            i++;
        } else if (strcmp(argv[i], "-h") == 0) {
            i++;
            print_usage(argv[0]);
            return 1;
        } else {
            fprintf(stderr, "Invalid option\n");
            print_usage(argv[0]);
            return 1;
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
    return 0;
}
