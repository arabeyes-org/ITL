#include "prayer.h"

calc_method_t calc_methods [] = {
 { MWL,  "Muslim World League (MWL)", ANGLE, 18, 17 },
 { ISNA, "Islamic Society of North America (ISNA)", ANGLE, 15, 15 },
 { EGAS, "Egyptian General Authority of Survey", ANGLE, 19.5, 17.5 },
 { UMAQ, "Umm Al-Qura University, Makkah", OFFSET, 18.5, 90 },
 { UIS,  "University of Islamic Sciences, Karachi", ANGLE, 18, 18 }
};

/* Helper geometric and mathematical funtions */
static double to_degrees(const double x);
static double to_radians(const double x);
static double arccot(const double x);
/* round is available only in C99 */
static double custom_round(const double x);
/* Normalizes the given value x within the range [0,N] */
static double normalize(const double x, const double N);

/* Get the Julian Day number of a given date */
static unsigned long get_julian_day_number(const struct tm *date);

/* Computes the approximate Sun coordinates for
   the given Julian Day Number jdn */
static void get_approx_sun_coord(const unsigned long jdn,
                                 struct approx_sun_coord *coord);

/* T function used to compute Sunrise, Sunset, Fajr and Isha
   Taken from: http://praytimes.org/calculation/
*/
static double T(const double alpha,
                const double latitude,
                const double D);

/* A function used to compute Asr
   Taken from: http://praytimes.org/calculation/
*/
static double A(const double t,
                const double latitude,
                const double D);

/* Methods for individual times and prayers */
static double get_dhuhr(const struct location *loc,
                        const struct approx_sun_coord *coord);

static double get_sunrise(const double dhuhr,
                          const struct location *loc,
                          const struct approx_sun_coord *coord);

static double get_fajr(const double dhuhr,
                       const double sunset,
                       const double sunrise,
                       const struct location *loc,
                       const struct approx_sun_coord *coord);

static double get_isha(const double dhuhr,
                       const double sunset,
                       const double sunrise,
                       const struct location *loc,
                       const struct approx_sun_coord *coord);

static void conv_time_to_event(const unsigned long julian_day,
                               const double decimal_time,
                               const round_t round,
                               event_t *event);



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
 * round is avalilable only in C99. Therefore, we use a custom one
 * @return The rounded value of x
 */
static double custom_round(const double x)
{
    return floor(x + 0.5);
}

/**
 * Compute the Julian Day Number for a given date
 * Source: https://en.wikipedia.org/wiki/Julian_day
 * @return The Julian day number
 */
static unsigned long get_julian_day_number(const struct tm *date)
{
    double a, y, m, jdn;

    assert (date != NULL);

    a = floor((14.0 - (double)(date->tm_mon + 1)) / 12.0);
    y = (double)(1900 + date->tm_year) + 4800.0 - a;
    m = (double)(date->tm_mon + 1) + 12.0 * a - 3.0;
    jdn = (double)date->tm_mday + \
          floor((153.0 * m + 2.0) / 5.0) + \
          365.0 * y + \
          floor(y / 4.0) - \
          floor(y / 100.0) + \
          floor(y / 400.0) - \
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
    assert (jdn > 2451545);

    d = (double)(jdn - 2451545); /* Remove the offset from jdn */
    g = 357.529 + 0.98560028 * d;
    q = 280.459 + 0.98564736 * d;
    L = q + 1.915 * sin(to_radians(g)) + \
        0.020 * sin(to_radians(2.0*g));
    R = 1.00014 - 0.01671 * cos(to_radians(g)) - \
        0.00014 * cos(to_radians(2.0*g));
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
    assert(loc->asr_method == SHAFII || loc->asr_method == HANAFI);

    switch(loc->asr_method) {
      case SHAFII:
        asr = dhuhr + A(1.0, loc->latitude, coord->D);
        break;
      case HANAFI:
        asr = dhuhr + A(2.0, loc->latitude, coord->D);
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

    return (dhuhr - T(0.8333 + 0.0347 * sqrt(loc->altitude),\
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

    return (dhuhr + T(0.8333 + 0.0347 * sqrt(loc->altitude),\
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
        if (loc->calc_method.isha_type == OFFSET) {
            /* Umm Al-Qura uses a fixed offset */
            isha = sunset + loc->calc_method.isha * ONE_MINUTE;
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
                               const round_t rounding,
                               event_t *t)
{
    double r = 0.0, f = 0.0;

    assert(julian_day > 0);
    assert(decimal_time >= 0.0 && decimal_time <= 24.0);
    assert(t != NULL);
    assert(rounding == UP ||
           rounding == DOWN ||
           rounding == NEAREST);

    t->julian_day = julian_day;
    f = floor(decimal_time);
    t->hour = (unsigned int)f;
    r = (decimal_time - f) * 60.0;
    switch (rounding) {
      case UP:
        t->minute = (unsigned int)(ceil(r));
        break;
      case DOWN:
        t->minute = (unsigned int)(floor(r));
        break;
      case NEAREST:
        t->minute = (unsigned int)(custom_round(r));
        break;
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
    unsigned long jdn, jdn_next, jdn_prev;
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

    fajr = get_fajr(true_noon, sunset_prev, sunrise, loc, &coord);
    dhuhr = true_noon;
    asr = get_asr(true_noon, loc, &coord);
    maghrib = sunset;
    isha = get_isha(true_noon, sunset, sunrise_next, loc, &coord);

    /* TODO: Find what Fiqh says regarding the rounding... */
    conv_time_to_event(jdn, fajr, NEAREST, &(pt->fajr));
    conv_time_to_event(jdn, sunrise, NEAREST, &(pt->sunrise));
    conv_time_to_event(jdn, dhuhr, NEAREST, &(pt->dhuhr));
    conv_time_to_event(jdn, asr, NEAREST, &(pt->asr));
    conv_time_to_event(jdn, maghrib, NEAREST, &(pt->maghrib));
    conv_time_to_event(jdn, isha, NEAREST, &(pt->isha));
}
