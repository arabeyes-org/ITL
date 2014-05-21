/************************************************************************
 *  Copyright (c) 2003-2006, 2009 Arabeyes, Thamer Mahmoud
 *
 *  A full featured Muslim Prayer Times calculator
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

#ifndef _ASTRO_
#define _ASTRO_

#include "prayer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defaults */
#define INVALID_TRIGGER 1
#define PI 3.1415926535898
#define CENTER_OF_SUN_ANGLE -0.833370 /* ..of sun's upper limb angle */
#define EARTH_RADIUS 6378140.0 /* Equatorial radius in meters */
#define ALTITUDE_REFRACTION 0.0347
#define DEG_TO_10_BASE 1/15.0

/* UTILITIES */
#define DEG_TO_RAD(A) ((A) * (PI/180.0))
#define RAD_TO_DEG(A) ((A) / (PI/180.0))


    typedef struct
    {
        double jd;      /* Astronomical Julian day (for local time with Delta-t) */
        double dec[3];  /* Declination */
        double ra[3];   /* Right Ascensions */
        double sid[3];  /* Apparent sidereal time */
        double dra[3];  /* Delta Right Ascensions */
        double rsum[3]; /* Sum of periodic values for radius vector R */
    } Astro ;

    /* Returns the astronomical Julian day (for local time with delta-t) */
    double getJulianDay(const Date* date, const double gmt);
    /* Fills the structure "astro" with a list of 3-day values, then checks and
     * updates these values if cached. The variable "tastro/topAstro" holds the
     * topocentric values of the same structure. */
    void getAstroValuesByDay(const double julianDay, const Location* loc,
                             Astro* astro, Astro* topAstro);
    double getTransit(const double lon, const Astro* tastro);
    double getSunrise(const Location* loc, const Astro* tastro);
    double getSunset(const Location* loc, const Astro* tastro);

#ifdef  __cplusplus
}
#endif

#endif  /* _ASTRO_ */
