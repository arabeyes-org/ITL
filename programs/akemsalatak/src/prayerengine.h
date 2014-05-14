/* prayerEngine.h */

/* *
 *   
 *   prayerEngine: A full featured Muslim Prayer Times calculator
 *   Copyright (C) 2003 Thamer Mahmoud (neokuwait@hotmail.com)
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later
 *   version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free
 *   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 * */


#ifndef _PRAYER_ENGINE_
#define _PRAYER_ENGINE_

#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif
        
        
/* Defaults */
#define PI 3.1415926535
#define NEAREST_LAT 48
#define EXTREME_LAT 49

        
  /* This holds the current date info. */
  typedef struct
  {
    int year;
    int month;
    int day;
  
  }  sDate ;
        
        
  /* This holds the location info. */
  typedef struct
  {
    double DegreeLong;    /* Longitude in decimal degree. */
    double DegreeLat;     /* Latitude in decimal degree. */
    double GMTdiff;       /* GMT difference. */
    double SeaLev;        /* Height above Sea level (in meters) */
    double DST;           /* Daylight savings time switch ( 0 if not used ).
                             Setting this to 1 should add 1 hour to all
                             the calculated prayer times */
  } sLocation ;
        
        
  /* ..And this holds the  calculation method used */
  typedef struct
  {
    double FajrAng;       /* Fajr angle */
    double IshaaAng;      /* Ishaa angle */
    double ImsaakAng;     /* Imsaak angle and fajr differance is 1.5 */
    int FajrINV;          /* Fajr Interval 90 or 120 (0 if not used) */
    int IshaaINV;         /* Ishaa Interval 90 or 120 (0 if not used) */
    int ImsaakINV;        /* Imsaak Interval from fajr is 10 minutes
                             if FajrINV is set */
    int Mathhab;          /* Shaf'i =1 and Hanafi =2 */
    int isExtreme;        /* Extreme latitude status (see below) */
  
  } sMethod ;
        
  /* 
     Supported methods for Extreme Latitude calculations 
     (sMethodData.isExtreme)
        
     0: none. if unable to calculate, leave as 0:00
     1: Nearest Latitude: All salats Always
     2: Nearest Latitude: Fajr Ishaa Always
     3: Nearest Latitude: Fajr Ishaa if invalid
     4: Nearest Good Day: All salats Always
     5: Nearest Good Day: Fajr Ishaa if invalid
     6: 1/7th of Night: Fajr Ishaa Always
     7: 1/7th of Night: Fajr Ishaa if invalid (should be default)
     8: Minutes from Shorooq/Maghrib: Always (e.g. Maghrib=Ishaa)
     9: Minutes from Shorooq/Maghrib: If invalid
  */
        
        
        
  /* The "GetPrayerTimes" fucntion fills the sPrayerTimes structure.
     This structure contains the prayer minutes and hours information
     like this:
        
     - nPrayerM[0] is today's Fajr minutes (notice the M)
     - nPrayerH[0] is today's Fajr Hours (notice the H)
     - ... and so on until...
     - nPrayerM[6] which is tomorrow's Fajr minutes
     - nPrayerM[7] which is today's Imsaak minutes
  */
  typedef struct 
  {
    int nPrayerM[8];
    int nPrayerH[8];
 
  } sPrayerTimes ;
        
                        

        
  /* main function */
  void GetPrayerTimes( sLocation, sMethod, sDate, sPrayerTimes*);
  /*  utilities */
  int GetDayofYear(int year, int month, int day);
  double Dms2Decimal(int deg, int min, int sec); 
  void Decimal2Dms(double dec, int *n1,int *n2, int *n3);
     
        
        
#ifdef  __cplusplus
}
#endif

#endif  /* _PRAYER_ENGINE_ */
