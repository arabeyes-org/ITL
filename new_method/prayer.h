/*
 Islamic Prayer Times and Qibla Direction Library
 By Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)
 Licensed under the BSD license shown in file LICENSE

 This is a "clean-room" implementation that uses simpler but
 approximate methods. This implementation should be easier
 to maintain and debug. However, we need to first to test it
 throughly before replacing the old method. Currently, it
 supports calculating the prayer times and qibla location for
 "normal" latitudes. However, "extreme latitude" methods are
 still missing.
*/

#ifndef PRAYER_H_
#define PRAYER_H_

/* Needed to expose POSIX stuff under C89 mode */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <assert.h>

/********** macros **********/

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

/* Latitude and Longitude of Ka'aba. Obtained using Google Maps */
#define KAABA_LATITUDE  (21.422491)
#define KAABA_LONGITUDE (39.826204)

/* One second and one minute as fractions of one hour
   Used for safety margins */
#define ONE_SECOND   (0.0002777777777777778) /* 1/3600 */
#define ONE_MINUTE   (0.016666666666666666)  /* 60/3600 */


/********** enum definitions **********/

/* Rounding method used for seconds */
typedef enum { UP, DOWN } round_t;
              
/* Asr Method */
typedef enum { SHAFII, HANAFI } asr_method_t;

/* Umm Al-Qura uses an offset instead of an angle */
typedef enum { ANGLE, OFFSET } isha_flag_t;

typedef enum { MWL=0, ISNA=1, EGAS=2, UMAQ=3, UIS=4 } method_id_t;

/* Extreme Latitude Method 
 * TODO: extreme latitudes are still WIP
 */
typedef enum { NONE=0,
               ANGLE_BASED=1,
               ONE_SEVENTH_OF_NIGHT=2,
               MIDDLE_OF_NIGHT=3
} extr_method_t;


/********** struct definitions **********/

/* The approximate Sun coordinates for a given Julian Day Number */
struct approx_sun_coord {
    double D;   /* Declination of the Sun */
    double EqT; /* The Equation of Time */
    double R;   /* The distance of the Sun from the Earth
                   in astronomical units (AU)  */
    double SD;  /* The angular semidiameter of the Sun in degrees */
};

typedef struct calc_method {
    method_id_t  id;         /* Method ID */
    char         name[64];   /* Full name of the method */
    double       fajr;       /* Fajr angle */
    isha_flag_t  isha_type;  /* Angle or offset? */
    double       isha;       /* Value for Isha angle/offset */
} calc_method_t;

/* Holds all the information related to the observer location */
struct location {
    char          name[64];    /* Observer's location name */
    double        longitude;   /* Observer's longitude */
    double        latitude;    /* Observer's latitude */
    double        height;      /* Observer's height in meters */
    double        timezone;    /* Observer's timezone (in hours) relative
                                  to Universal Coordinated Time (UTC) */
    int           daylight;    /* Daylight Savings Time (DST) Flag
                                  Set to 1 if DST is on, 0 otherwise */
    asr_method_t  asr_method;  /* Asr Method: Shafii or Hanafi */
    calc_method_t calc_method; /* Fajr and Isha Calculation method */
    extr_method_t extr_method; /* Extreme latitude method */
};

/* Holds the time of a single event (i.e., prayer or sunrise) */
typedef struct event {
    unsigned long   julian_day;  /* The Julian day of the event */
    unsigned int    hour;        /* Hour */
    unsigned int    minute;      /* Minute */
} event_t;

/* Holds all the prayers/events times of a given day */
struct prayer_times {
    event_t fajr;
    event_t sunrise;
    event_t dhuhr;
    event_t asr;
    event_t maghrib;
    event_t isha;
};

calc_method_t calc_methods [] = {
 { MWL,  "Muslim World League (MWL)" , 18, ANGLE, 17 },
 { ISNA, "Islamic Society of North America (ISNA)", 15, ANGLE, 15 },
 { EGAS, "Egyptian General Authority of Survey", 19.5, ANGLE, 17.5 },
 { UMAQ, "Umm Al-Qura University, Makkah", 18.5, OFFSET, 90 },
 { UIS,  "University of Islamic Sciences, Karachi", 18, ANGLE, 18 }
};


/********** static functions **********/

/* Helper geometric funtions */
static double to_degrees(const double x);
static double to_radians(const double x);
static double arccot(const double x);

/* Normalizes the given value x with the range [0,N] */
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

static void set_location(struct location *loc,
                         struct tm *date);

/* Public functions */
void get_prayer_times(const struct tm *day,
                      const struct location *loc,
                      struct prayer_times *pt);

double get_qibla_direction(const struct location *loc);


#endif
