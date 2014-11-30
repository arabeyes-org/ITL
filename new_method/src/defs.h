#ifndef _DEFS_H_
#define _DEFS_H_

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

/* Latitude and Longitude of Ka'aba. Obtained using Google Maps */
#define KAABA_LATITUDE  (21.42249)
#define KAABA_LONGITUDE (39.82620)

/* One second and one minute as fractions of one hour
   Used for safety margins */
#define ONE_SECOND   (1.0/3600.0)
#define ONE_MINUTE   (60.0/3600.0)


/* Rounding method used for seconds */
typedef enum { UP, DOWN, NEAREST } round_t;
              
/* Asr Method */
typedef enum { SHAFII=0, HANAFI=1 } asr_method_t;

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


/* The approximate Sun coordinates for a given Julian Day Number */
struct approx_sun_coord {
    double D;   /* Declination of the Sun */
    double EqT; /* The Equation of Time */
    double R;   /* The distance of the Sun from the Earth
                   in astronomical units (AU)  */
    double SD;  /* The angular semidiameter of the Sun in degrees */
};

/* The order of struct members below guarantees
 * that the compiler doesn't add padding bytes automatically
 * on 64-bit architectures */
typedef struct calc_method {
    method_id_t  id;         /* Method ID */
    char         name[48];   /* Full name of the method */
    isha_flag_t  isha_type;  /* Angle or offset? */
    double       fajr;       /* Fajr angle */
    double       isha;       /* Value for Isha angle/offset */
} calc_method_t;

/* Holds all the information related to the observer location
 * The struct members ordering guarantees that the compiler does
 * not add any padding bytes when compiling for 64-bit machines */
struct location {
    double        longitude;   /* Observer's longitude */
    double        latitude;    /* Observer's latitude */
    double        altitude;    /* Observer's altitude in meters */
    double        timezone;    /* Observer's timezone (in hours) relative
                                  to Universal Coordinated Time (UTC) */
    int           daylight;    /* Daylight Savings Time (DST) Flag
                                  Set to 1 if DST is on, 0 otherwise */
    asr_method_t  asr_method;  /* Asr Method: Shafii or Hanafi */
    calc_method_t calc_method; /* Fajr and Isha Calculation method */
    extr_method_t extr_method; /* Extreme latitude method */
    char          name[60];    /* Observer's location name */
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

#endif
