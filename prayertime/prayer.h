/************************************************************************
 *  Copyright (c) 2003-2006, 2009, Arabeyes, Thamer Mahmoud
 *
 *  A full featured Muslim Prayer Times calculator
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

#ifndef _PRAYER_ENGINE_
#define _PRAYER_ENGINE_

#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif


    /* This holds the current date info. */
    typedef struct
    {
        int day;
        int month;
        int year;

    } Date ;


    /* This holds the location info. */
    typedef struct
    {
        double degreeLong;  /* Longitude in decimal degree. */
        double degreeLat;   /* Latitude in decimal degree. */
        double gmtDiff;     /* GMT difference at regular time. */
        int dst;            /* Daylight savings time switch (0 if not used).
                               Setting this to 1 should add 1 hour to all the
                               calculated prayer times */
        double seaLevel;    /* Height above Sea level in meters */
        double pressure;    /* Atmospheric pressure in millibars (the
                               astronomical standard value is 1010) */
        double temperature; /* Temperature in Celsius degree (the astronomical
                               standard value is 10) */
    } Location ;


    /* This structure holds the calculation method used. NOTE: Before explicitly
       setting any of these values, it is more safe to default initialize them
       by calling 'getMethod(0, &method)' */
    typedef struct
    {
        int method;         /* Chosen calculation method */
        double fajrAng;     /* Fajr angle */
        double ishaaAng;    /* Ishaa angle */
        double imsaakAng;   /* The angle difference between Imsaak and Fajr (
                               default is 1.5)*/
        int fajrInv;        /* Fajr Interval is the amount of minutes between
                               Fajr and Shurooq (0 if not used) */
        int ishaaInv;       /* Ishaa Interval is the amount if minutes between
                               Ishaa and Maghrib (0 if not used) */
        int imsaakInv;      /* Imsaak Interval is the amount of minutes between
                               Imsaak and Fajr. The default is 10 minutes before
                               Fajr if Fajr Interval is set */
        int round;          /* Method used for rounding seconds:
                               0: No Rounding. "Prayer.seconds" is set to the
                                  amount of computed seconds.
                               1: Normal Rounding. If seconds are equal to
                                  30 or above, add 1 minute. Sets
                                  "Prayer.seconds" to zero.
                               2: Special Rounding. Similar to normal rounding
                                  but we always round down for Shurooq and
                                  Imsaak times. (default)
                               3: Aggressive Rounding. Similar to Special
                                  Rounding but we add 1 minute if the seconds
                                  value is equal to 1 second or more.  */
        int mathhab;        /* Assr prayer shadow ratio:
                               1: Shaf'i (default)
                               2: Hanafi */
        double nearestLat;  /* Latitude Used for the 'Nearest Latitude' extreme
                               methods. The default is 48.5 */
        int extreme;        /* Extreme latitude calculation method (see
                               below) */
        double extremeLat;  /* Latitude at which the extreme method should
                               always be used. The default is 55 */
        int offset;         /* Enable Offsets switch (set this to 1 to
                               activate). This option allows you to add or
                               subtract any amount of minutes to the daily
                               computed prayer times based on values (in
                               minutes) for each prayer in the offList array */
        double offList[6];  /* For Example: If you want to add 30 seconds to
                               Maghrib and subtract 2 minutes from Ishaa:
                               offset = 1
                               offList[4] = 0.5
                               offList[5] = -2
                               ..and than call getPrayerTimes as usual. */

    } Method ;


    /*
      Supported methods for Extreme Latitude calculations (Method.extreme):

      0:  none. if unable to calculate, leave as 99:99
      1:  Nearest Latitude: All prayers Always
      2:  Nearest Latitude: Fajr Ishaa Always
      3:  Nearest Latitude: Fajr Ishaa if invalid
      4:  Nearest Good Day: All prayers Always
      5:  Nearest Good Day: Fajr Ishaa if invalid (default)
      6:  1/7th of Night: Fajr Ishaa Always
      7:  1/7th of Night: Fajr Ishaa if invalid
      8:  1/7th of Day: Fajr Ishaa Always
      9:  1/7th of Day: Fajr Ishaa if invalid
      10: Half of the Night: Fajr Ishaa Always
      11: Half of the Night: Fajr Ishaa if invalid
      12: Minutes from Shorooq/Maghrib: Fajr Ishaa Always (e.g. Maghrib=Ishaa)
      13: Minutes from Shorooq/Maghrib: Fajr Ishaa If invalid
      14: Nearest Good Day: Fajr Ishaa if either is invalid
      15: Angle based: Fajr Ishaa if invalid
    */


    /* This function is used to auto fill the Method structure with predefined
       data. The supported auto-fill methods for calculations at normal
       circumstances are:

       0: none. Set to default or 0
       1: Egyptian General Authority of Survey
       2: University of Islamic Sciences, Karachi (Shaf'i)
       3: University of Islamic Sciences, Karachi (Hanafi)
       4: Islamic Society of North America
       5: Muslim World League (MWL)
       6: Umm Al-Qurra, Saudi Arabia
       7: Fixed Ishaa Interval (always 90)
       8: Egyptian General Authority of Survey (Egypt)
       9: Umm Al-Qurra Ramadan, Saudi Arabia
      10: Moonsighting Committee Worldwide
      11: Morocco Awqaf, Morocco
    */
    void getMethod(int n, Method* conf);


    /* This structure holds the prayer time output for a single prayer. */
    typedef struct
    {
        int hour;       /* prayer time hour */
        int minute;     /* prayer time minute */
        int second;     /* prayer time second */
        int isExtreme;  /* Extreme calculation status. The 'getPrayerTimes'
                           function sets this variable to 1 to indicate that
                           this particular prayer time has been calculated
                           through extreme latitude methods and NOT by
                           conventional means of calculation. */
    } Prayer ;


    /* The "getPrayerTimes" function fills an array of six Prayer structures
       "Prayer[6]". This list contains the prayer minutes and hours information
       like this:

       - Prayer[0].minute    is today's Fajr minutes
       - Prayer[1].hour      is today's Shorooq hours
       - ... and so on until...
       - Prayer[5].minute    is today's Ishaa minutes
    */
    void getPrayerTimes (const Location*, const Method*, const Date*, Prayer*);


    /* Extended prayer times */
    void getImsaak (const Location*, const Method*, const Date*, Prayer*);
    void getNextDayImsaak (const Location*, const Method*, const Date*, Prayer*);
    void getNextDayFajr (const Location*, const Method*, const Date*, Prayer*);


    /* utilities */
    int getDayofYear (int year, int month, int day);
    double dms2Decimal (int deg, int min, double sec, char dir);
    void decimal2Dms (double decimal, int *deg, int *min, double *sec);


    /* Qibla */
    double getNorthQibla(const Location* location);



#ifdef  __cplusplus
}
#endif

#endif  /* _PRAYER_ENGINE_ */
