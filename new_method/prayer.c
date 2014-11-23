#include "prayer.h"

/**
 * Convert a given angle specified in degrees into radians
 */
static double to_radians(const double x)
{
    return ((x) * M_PI) / 180.0;
}


/**
 * Convert a given angle specified in radians into degrees
 */
static double to_degrees(const double x)
{
    return ((x) * 180.0) / M_PI;
}


/**
 * Compute the arccotangent of a given value
 */
static double arccot(const double x)
{
    return atan2(1.0, (x));
}


/**
 * Normalizes the given value x to be within the range [0,N]
 */
static double normalize(const double x, const double N)
{
    double n;
    assert(N > 0.0);
    n = x - (N * floor( x / N ));
    assert(n <= N);
    return n;
}

/**
 * Compute the Julian Day Number for a given date
 * Source: https://en.wikipedia.org/wiki/Julian_day
 */
static unsigned long get_julian_day_number(const struct tm *date)
{
    double a = 0.0, y = 0.0, m = 0.0, jdn = 0.0;

    assert (date != NULL);

    a = floor((14.0 - (double)(date->tm_mon + 1)) / 12.0);
    y = (double)(1900 + date->tm_year) + 4800.0 - a;
    m = (double)(date->tm_mon + 1) + 12.0*a - 3.0;
    jdn = (double)date->tm_mday + \
          floor((153.0*m + 2.0) / 5.0) + \
          365.0*y + \
          floor(y/4.0) - \
          floor(y/100.0) + \
          floor(y/400.0) - \
          32045.0;

    return (unsigned long)jdn;
}


/**
 * Compute the approximate Sun coordinates from the given Julian Day
 * Number jdn according to the method given the US Naval Observatory (USNO)
 * on: http://aa.usno.navy.mil/faq/docs/SunApprox.php
 * The computed values are stored in struct coord.
 */
static void get_approx_sun_coord(const unsigned long jdn,
                                 struct approx_sun_coord *coord)
{
    double d;   /* Julian Day Number from 1 January 2000 */
    double g;   /* Mean anomaly of the Sun */
    double q;   /* Mean longitude of the Sun */
    double L;   /* Geocentric apparent ecliptic longitude of the Sun
                   (adjusted for aberration) */
    double R;   /* The approximated distance of the Sun from the Earth
                   in astronomical units (AU) */
    double e;   /* The mean obliquity of the ecliptic */
    double RA;  /* The Sun's right ascension */
    double D;   /* The Sun's Declination */
    double EqT; /* The Equation of Time */
    double SD;  /* The angular semidiameter of the Sun in degrees */

    assert (coord != NULL);

    d = jdn - 2451545.0; /* Remove the offset from jdn */
    g = 357.529 + 0.98560028 * d;
    q = 280.459 + 0.98564736 * d;
    L = q + 1.915 * sin(to_radians(g)) + 0.020 * sin(to_radians(2*g));
    R = 1.00014 - 0.01671 * cos(to_radians(g)) - \
        0.00014 * cos(to_radians(2*g));
    e = 23.439 - 0.00000036 * d;
    RA = to_degrees(atan2(cos(to_radians(e)) * sin(to_radians(L)), \
                          cos(to_radians(L)))/15.0);
    D = to_degrees(asin(sin(to_radians(e)) * sin(to_radians(L))));
    EqT = q/15.0 - RA;

    /* Resulting EqT Can be larger than 360.
       Therefore, it needs normalization  */
    EqT = normalize(EqT, 360.0);
    SD = 0.2666 / R;

    coord->D = D;
    coord->EqT = EqT;
    coord->R = R;
    coord->SD = SD;
}


/**
 * T function used to compute Sunrise, Sunset, Fajr, and Isha
 * Taken from: http://praytimes.org/calculation/
 */
static double T(const double alpha,
                const double latitude,
                const double D)
{
    double p1 = 1.0/15.0;
    double p2 = cos(to_radians(latitude)) * cos(to_radians(D));
    double p3 = sin(to_radians(latitude)) * sin(to_radians(D));
    double p4 = -1.0 * sin(to_radians(alpha));
    double p5 = to_degrees(acos((p4 - p3) / p2));
    double r = p1 * p5;
    return r;
}


/**
 * A function used to compute Asr
 * Taken from: http://praytimes.org/calculation/
 */
static double A(const double t,
                const double latitude,
                const double D)
{
    double p1 = 1.0/15.0;
    double p2 = cos(to_radians(latitude)) * cos(to_radians(D));
    double p3 = sin(to_radians(latitude)) * sin(to_radians(D));
    double p4 = tan(to_radians((latitude - D)));
    double p5 = arccot(t + p4);
    double p6 = sin(p5);
    double p7 = acos((p6 - p3) / p2);
    double r = p1 * to_degrees(p7);
    return r;
}


/**
 * Compute the Dhuhr prayer time
 */
static double get_dhuhr(const struct location *loc,
                        const struct approx_sun_coord *coord)
{
    double dhuhr = 0.0;

    assert(loc != NULL);
    assert(coord != NULL);

    dhuhr = 12.0 + loc->timezone - loc->longitude/15.0 - coord->EqT;
    dhuhr = normalize(dhuhr, 24.0);
    return dhuhr;
}


/**
 * Compute the Asr prayer time
 */
static double get_asr(const double dhuhr,
                      const struct location *loc,
                      const struct approx_sun_coord *coord)
{
    double asr = 0.0;

    assert(dhuhr > 0.0);
    assert(loc != NULL);
    assert(coord != NULL);

    switch(loc->asr_method) {
    case SHAFII:
        asr = dhuhr + A(1.0, loc->latitude, coord->D);
        break;
    case HANAFI:
        asr = dhuhr + A(2.0, loc->latitude, coord->D);
        break;
    default:
        fprintf(stderr, "Error! Invalid Asr method detected\n");
        exit(EXIT_FAILURE);
        break;
    }
    asr = normalize(asr, 24.0);
    return asr;
}


/**
 * Compute the Sunrise time
 * TODO: Check if this is valid for extreme altitudes
 */
static double get_sunrise(const double dhuhr,
                          const struct location *loc,
                          const struct approx_sun_coord *coord)
{
    assert(loc != NULL);
    assert(coord != NULL);

    return (dhuhr - T(0.8333 + 0.0347 * sqrt(loc->height),\
                      loc->latitude, coord->D));
}


/**
 * Compute the Sunset time
 * TODO: Check if this is valid for extreme altitudes
 */
static double get_sunset(const double dhuhr,
                         const struct location *loc,
                         const struct approx_sun_coord *coord)
{
    assert(loc != NULL);
    assert(coord != NULL);

    return (dhuhr + T(0.8333 + 0.0347 * sqrt(loc->height),\
                      loc->latitude, coord->D));
}


/**
 * Compute the Fajr prayer time
 */
static double get_fajr(const double dhuhr,
                       const double sunset,
                       const double sunrise,
                       const struct location *loc,
                       const struct approx_sun_coord *coord)
{
    double fajr = 0.0;

    (void)sunset;
    (void)sunrise;

    assert(dhuhr > 0.0);
    assert(loc != NULL);
    assert(coord != NULL);

    if (loc->extr_method == NONE) {
        /* Normal latitude. Use the classical methods */
        fajr = dhuhr - T(loc->calc_method.fajr,\
                         loc->latitude,\
                         coord->D);
    } else {
        /* TODO: Extreme methods */
        fprintf(stderr, "Extreme methods are not implemented yet\n");
        exit(EXIT_FAILURE);        
    }

    return fajr;
}


/**
 * Compute the Isha prayer time
 */
static double get_isha(const double dhuhr,
                       const double sunset,
                       const double sunrise,
                       const struct location *loc,
                       const struct approx_sun_coord *coord)
{
    double isha = 0.0;

    (void)sunset;
    (void)sunrise;

    assert(dhuhr > 0.0);
    assert(loc != NULL);
    assert(coord != NULL);

    if (loc->extr_method == NONE) {
        if (loc->calc_method.id == UMAQ) {
            /* Umm Al-Qura uses a fixed offset */
            isha = sunset + 90.0 * ONE_MINUTE;
        } else {
            isha = dhuhr + T(loc->calc_method.isha,\
                             loc->latitude,\
                             coord->D);
        }
    } else {
        /* TODO: Extreme latitude */
        fprintf(stderr, "Extreme altitudes are not implemented yet!\n");
        exit(EXIT_FAILURE);
    }

    isha = normalize(isha, 24.0);
    return isha;
}


/**
 * Convert a given time in decimal format to
 * hh:mm format and store it in the given event struct.
 * The minutes part can be rounded up or down based on the
 * round flag
 */
static void conv_time_to_event(const unsigned long julian_day,
                               const double decimal_time,
                               const round_t round,
                               event_t *t)
{
    double r = 0.0, f = 0.0;

    assert(julian_day > 0);
    assert(decimal_time >= 0.0 && decimal_time <= 24.0);
    assert(t != NULL);

    t->julian_day = julian_day;
    f = floor(decimal_time);
    t->hour = (unsigned int)f;
    r = (decimal_time - f) * 60.0 * 60.0;
    if (round == UP) {
        t->minute = (unsigned int)ceil(r / 60.0);
    } else {
        t->minute = (unsigned int)floor(r / 60.0);
    }
}

/*
 * Taken from: http://stackoverflow.com/a/122721
 */
static char * trim_whitespace(char * str)
{
    char * end;

    assert(str != NULL);

    while(isspace(*str)) str++;
    if (*str == '\0') return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;

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
        key = trim_whitespace(key);
        value = strtok(NULL, delimiter);
        value = trim_whitespace(value);
        r = add_key_value(key, value, loc);
        if (r != EXIT_SUCCESS) {
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
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


static void parse_arguments(int argc,
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

/**
 * Compute the Qibla direction from North clockwise
 * using the Equation (1) given in references/qibla.pdf
 */
double get_qibla_direction(const struct location *loc)
{
    double p1, p2, p3, qibla;

    assert(loc != NULL);

    p1 = sin(to_radians(KAABA_LONGITUDE - loc->longitude));
    p2 = cos(to_radians(loc->latitude)) * \
         tan(to_radians(KAABA_LATITUDE));
    p3 = sin(to_radians(loc->latitude)) * \
         cos(to_radians(KAABA_LONGITUDE - loc->longitude));
    qibla = to_degrees(atan2(p1, (p2 - p3)));

    return qibla;
}


void get_prayer_times(const struct tm *date,
                      const struct location *loc,
                      struct prayer_times *pt)
{
    double jdn, jdn_next, jdn_prev;
    double true_noon, sunrise, sunset;
    double noon_next, sunrise_next;
    double noon_prev, sunset_prev;
    double fajr, dhuhr, asr, maghrib, isha;
    struct approx_sun_coord coord, coord_next, coord_prev;

    assert(date != NULL);
    assert(loc != NULL);
    assert(pt != NULL);

    jdn = get_julian_day_number(date);
    jdn_next = jdn + 1;
    jdn_prev = jdn - 1;

    get_approx_sun_coord(jdn, &coord);
    get_approx_sun_coord(jdn_next, &coord_next);
    get_approx_sun_coord(jdn_prev, &coord_prev);

    true_noon = get_dhuhr(loc, &coord);
    noon_next = get_dhuhr(loc, &coord_next);
    noon_prev = get_dhuhr(loc, &coord_prev);

    sunrise = get_sunrise(true_noon, loc, &coord);
    sunset = get_sunset(true_noon, loc, &coord);
    sunrise_next = get_sunrise(noon_next, loc, &coord_next);
    sunset_prev = get_sunset(noon_prev, loc, &coord_prev);

    asr = get_asr(true_noon, loc, &coord);
    fajr = get_fajr(true_noon, sunset_prev, sunrise, loc, &coord);
    isha = get_isha(true_noon, sunset, sunrise_next, loc, &coord);

    /* Compute the safety margins for prayers */
    /* Dhuhr has a 65 seconds safety margin */
    dhuhr = true_noon + ONE_MINUTE + 5.0 * ONE_SECOND;
    /* Maghrib has between 1-3 minutes safety margin */
    maghrib = sunset + ONE_MINUTE;

    /* TODO: I choose the UP, DOWN flags to ensure
       higher safety margin for someone who is fasting */
    conv_time_to_event(jdn, fajr, DOWN, &(pt->fajr));
    conv_time_to_event(jdn, sunrise, DOWN, &(pt->sunrise));
    conv_time_to_event(jdn, dhuhr, UP, &(pt->dhuhr));
    conv_time_to_event(jdn, asr, UP, &(pt->asr));
    conv_time_to_event(jdn, maghrib, UP, &(pt->maghrib));
    conv_time_to_event(jdn, isha, UP, &(pt->isha));
}


int main(int argc, char **argv)
{
    struct location loc;
    struct tm date;
    time_t t;
    double qibla;
    struct prayer_times pt;

    parse_arguments(argc, argv, &loc);

    /* Get the date */
    time(&t);
    localtime_r(&t, &date);

    fprintf(stdout, "Computing Prayer Times on %s", asctime(&date));
    fprintf(stdout, "Current location is: %s "
            "(Latitude = %f, Longitude = %f)\n",
            loc.name, loc.latitude, loc.longitude);

    qibla = get_qibla_direction(&loc);
    fprintf(stdout, "Qibla direction is %f degrees"
                    " from North clockwise\n", qibla);

    fprintf(stdout, "Current Timezone is UTC%s%.2f"
                    " (daylight is %s)\n",
            (loc.timezone >= 0? "+" : "-"),
            loc.timezone, (loc.daylight == 1? "on" : "off"));
    fprintf(stdout, "Calculation Method Used: %s, Fajr angle: %.2f,"
            " Isha %s: %.2f\n",
            loc.calc_method.name, loc.calc_method.fajr,
            (loc.calc_method.isha_type == ANGLE ?\
             "angle" : "offset"),
            loc.calc_method.isha);

    get_prayer_times(&date, &loc, &pt);

    fprintf(stdout, "\n Fajr\t\tSunrise\t\tDhuhr"
                    "\t\tAsr\t\tSunset\t\tIsha\n");
    fprintf(stdout, "--------------------------"
                    "--------------------------");
    fprintf(stdout, "---------------------------------\n");
    fprintf(stdout, " %d:%02d\t\t%d:%02d\t\t%d:%02d\t\t"
            "%d:%02d\t\t%d:%02d\t\t%d:%02d\n\n",
            pt.fajr.hour, pt.fajr.minute,
            pt.sunrise.hour, pt.sunrise.minute,
            pt.dhuhr.hour, pt.dhuhr.minute,
            pt.asr.hour, pt.asr.minute,
            pt.maghrib.hour, pt.maghrib.minute,
            pt.isha.hour, pt.isha.minute);


    return EXIT_SUCCESS;
}
