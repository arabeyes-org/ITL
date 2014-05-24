/************************************************************************
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  This is a demo file to note how to call the upcoming hijri library.
 *  It is envisioned that both hijri and umm_alqura will be within the
 *  same library and will be triggered via a command-line (ie. passed-in
 *  flag or indicator).
 *
 * (www.arabeyes.org - under GPL license)
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>               /* for time_t */
#include "hijri.h"

/* Various interesting/relevant sites

  old.code -> http://emr.cs.uiuc.edu/~reingold/calendar.C
  http://www.rabiah.com/convert/convert.php3
  http://bennyhills.fortunecity.com/elfman/454/calindex.html#TOP
  http://www.ori.unizh.ch/hegira.html
  http://prayer.al-islam.com/convert.asp?l=eng

  http://fisher.osu.edu/~muhanna_1/Muhanna.html
  http://www.math.nus.edu.sg/aslaksen/calendar/links.shtml#Islamic
  http://www.phys.uu.nl/~vgent/islam/mecca/ummalqura.htm
 */

/*
 Sample demo file - show basics (very preliminary calls)
 */
int main(void) 
{
   /* hijri code specifics */
   int day, month, year;
   sDate mydate;
   sDate mydate2;
   int i;
   int error_code = 0;

   time_t mytime;
   struct tm *t_ptr;

   /* Get current dte structure */
   time(&mytime);

   t_ptr = localtime(&mytime);

   /* Set current time values */
   day   = t_ptr->tm_mday;
   month = t_ptr->tm_mon  + 1;
   year	 = t_ptr->tm_year + 1900;

   /* Convert using hijri code from meladi to hijri */
   error_code = h_date(&mydate, day, month, year);

   if (error_code)
      printf("Got an error from the library (code = %d)", error_code);
  
   printf("Current date (dd/mm/yyyy):\n");
   printf("+ Gregorian Input  - %2d/%2d/%4d\n", day, month, year);
   printf("                   - %s(%s) - %s(%s) %2d, %4d\n", mydate.frm_dname,
	  mydate.frm_dname_sh, mydate.frm_mname, mydate.frm_mname_sh,
	  day, year);
   printf("+ Hijri/Islamic    - %2d/%2d/%4d\n", mydate.day,
	  mydate.month, mydate.year);
   printf("                   - %s(%s) - %s(%s) %2d, %4d A.H\n",
	  mydate.to_dname, mydate.to_dname_sh,mydate.to_mname,
	  mydate.to_mname_sh, mydate.day, mydate.year);

   for (i = 0; mydate.event[i] != NULL; i++)
   {
      printf("  Day's Event      - %s\n", mydate.event[i]);
   }
   free(mydate.event);

   error_code = g_date(&mydate2, mydate.day, mydate.month, mydate.year);

   if (error_code)
      printf("Got an error from the library (code = %d)", error_code);
  

   printf("+ Gregorian Conv.  - %2d/%2d/%4d\n", mydate2.day,
	  mydate2.month, mydate2.year);
   printf("                   - %s(%s) - %s(%s) %2d, %4d A.H\n",
	  mydate2.to_dname, mydate2.to_dname_sh,mydate2.to_mname,
	  mydate2.to_mname_sh, mydate2.day, mydate2.year);

   printf("\n");

   /* Tests for umm_alqura code */
   printf("Umm-AlQura results:\n");

   G2H(&mydate, day, month, year);
   printf("G2H (to Hijri)     - %d/%d/%d\n", mydate.day,
	  mydate.month, mydate.year);
   printf("                   - day of week is %d\n", mydate.weekday);

   H2G(&mydate2, mydate.day, mydate.month, mydate.year);
   printf("H2G (to Gregorian) - %d/%d/%d\n", mydate2.day,
	  mydate2.month, mydate2.year);
   printf("                   - day of week is %d\n", mydate2.weekday);

   return(0);
}
