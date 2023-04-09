/************************************************************************
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  A Hijri (Islamic) to/from Gregorian (Christian) date conversion library.
 *
 * (*)NOTE: A great deal of inspiration as well as algorithmic insight was
 *          based on the lisp code from GNU Emacs' cal-islam.el which itself
 *          is baed on ``Calendrical Calculations'' by Nachum Dershowitz and
 *          Edward M. Reingold.
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hijri.h"

#define PROG_NAME       "hijri";

/* Various Islamic/Hijri Important Events */
sEvent h_events_table[] =
{
   /* d   m     Event String */
   {  1,  1, "Islamic New Year"},
   { 15,  1, "Battle of Qadisiah (14 A.H)"},
   { 10,  1, "Aashura"},
   { 10,  2, "Start of Omar ibn Abd Al-Aziz Khilafah (99 A.H)"},
   {  4,  3, "Start of Islamic calander by Omar Ibn Al-Khattab (16 A.H)"},
   { 12,  3, "Birth of the Prophet (PBUH)"},
   { 20,  3, "Liberation of Bait AL-Maqdis by Omar Ibn Al-Khattab (15 A.H)"},
   { 25,  4, "Battle of Hitteen (583 A.H)"},
   {  5,  5, "Battle of Muatah (8 A.H)"},
   { 27,  7, "Salahuddin liberates Bait Al-Maqdis from crusaders"},
   { 27,  7, "Al-Israa wa Al-Miaaraj"},
   {  1,  9, "First day of month-long Fasting"},
   { 17,  9, "Battle of Badr (2 A.H)"},
   { 21,  9, "Liberation of Makkah (8 A.H)"},
   { 21,  9, "Quran Revealed - day #1"},
   { 22,  9, "Quran Revealed - day #2"},
   { 23,  9, "Quran Revealed - day #3"},
   { 24,  9, "Quran Revealed - day #4"},
   { 25,  9, "Quran Revealed - day #5"},
   { 26,  9, "Quran Revealed - day #6"},
   { 27,  9, "Quran Revealed - day #7"},
   { 28,  9, "Quran Revealed - day #8"},
   { 29,  9, "Quran Revealed - day #9"},
   {  1, 10, "Eid Al-Fitr"},
   {  6, 10, "Battle of Uhud (3 A.H)"},
   { 10, 10, "Battle of Hunian (8 A.H)"},
   {  8, 12, "Hajj to Makkah - day #1"},
   {  9, 12, "Hajj to Makkah - day #2"},
   {  9, 12, "Day of Arafah"},
   { 10, 12, "Hajj to Makkah - day #3"},
   { 10, 12, "Eid Al-Adhaa - day #1"},
   { 11, 12, "Eid Al-Adhaa - day #2"},
   { 12, 12, "Eid Al-Adhaa - day #3"},
};

/* Absolute date of start of Islamic calendar (July 19, 622 Gregorian)*/
const int HijriEpoch		= 227015;
const int GregorianEpoch	= 1;

/* Various user day/month tangibles */
char *g_day[7] = {"Sunday", "Monday", "Tuesday", "Wednesday",
		  "Thursday", "Friday", "Saturday"};

char *h_day[7] = {"Ahad", "Ithnain", "Thulatha", "Arbiaa",
		  "Khamees", "Jumaa", "Sabt"};

char *g_day_short[7] = {"Sun", "Mon", "Tue", "Wed",
			"Thu", "Fri", "Sat"};

char *h_day_short[7] = {"Ahd", "Ith", "Tha", "Arb",
			"Kha", "Jum", "Sab"};

char *g_month[13] = {"skip",
		     "January", "February", "March", "April",
		     "May", "June", "July", "August",
		     "September", "October", "November", "December"};

char *h_month[13] = {"skip",
		     "Muharram", "Safar", "Rabi I", "Rabi II",
		     "Jumada I", "Jumada II", "Rajab", "Shaaban",
		     "Ramadan", "Shawwal", "Thul-Qiaadah", "Thul-Hijja"};

char *g_month_short[13] = {"skip",
			   "Jan", "Feb", "Mar", "Apr",
			   "May", "Jun", "Jul", "Aug",
			   "Sep", "Oct", "Nov", "Dec"};

char *h_month_short[13] = {"skip",
			   "Muh", "Saf", "Rb1", "Rb2",
			   "Jm1", "Jm2", "Raj", "Sha",
			   "Ram", "Sha", "Qid", "Hij"};


/* Forward declarations for C99 compatibility.  */
int g_absolute(int day, int month, int year);
int g_numdays(int month, int year);

/* Store-off any events for passed-in date */
int
get_events(char ***addr_event,
	   sEvent *events_table,
	   int array_size,
	   int day,
	   int month)
{
   int table_depth;
   int count;
   int found	= 0;
   int ptr_size	= sizeof(char *);
   char **arr_ptrs;

   /* Calculate the table's depth for iternation count */
   table_depth = (array_size / sizeof(sEvent));

   /* Traverse teh table to see if there are events matching passed-in date */
   for (count = 0; count < table_depth; count++)
   {
      if ((events_table[count].day   == day) &&
	  (events_table[count].month == month))
      {
	 found++;
      }
   }

   /* Allocate memory for the return pointer(s)
      - we'll always need at least 1 for NULL
    */
   arr_ptrs = malloc((ptr_size * found) + ptr_size);

   if (arr_ptrs == NULL)
   {
      char *pname = PROG_NAME;
      printf("%s: Exiting, can't malloc/attain memory !\n", pname);
      return(10);
   }

   /* Populate those entries that were found (if any) */
   if (found)
   {
      /* Reinitiate and now populate newly malloced memory with events */
      found = 0;
      for (count = 0; count < table_depth; count++)
      {
	 if ((events_table[count].day   == day) &&
	     (events_table[count].month == month))
	 {
	    arr_ptrs[found++] = events_table[count].event;
	 }
      }
   }
   /* Set my ending entry (for stopage and empty sets) */
   arr_ptrs[found] = NULL;

   /* Assign contents of the pointer to new address */
   *addr_event = arr_ptrs;

   return(0);
}


/* Fill-in the rest of the sDate struct with various nicities */
int
fill_datestruct(sDate *fdate,
		int weekday,
		int frm_month_num,
		int to_month_num,
		char *frm_day[],
		char *frm_day_short[],
		char *frm_month[],
		char *frm_month_short[],
		char *to_day[],
		char *to_day_short[],
		char *to_month[],
		char *to_month_short[],
		sEvent *farr_table,
		int farr_size)

{
   int error_event;

   fdate->frm_dname	= (char *) frm_day[weekday];
   fdate->frm_dname_sh	= (char *) frm_day_short[weekday];
   fdate->frm_mname	= (char *) frm_month[frm_month_num];
   fdate->frm_mname_sh	= (char *) frm_month_short[frm_month_num];

   fdate->to_dname	= (char *) to_day[weekday];
   fdate->to_dname_sh	= (char *) to_day_short[weekday];
   fdate->to_mname	= (char *) to_month[to_month_num];
   fdate->to_mname_sh	= (char *) to_month_short[to_month_num];

   if (to_month_num == 12)
      fdate->to_mname2	= (char *) to_month[1];
   else
      fdate->to_mname2	= (char *) to_month[to_month_num+1];

   error_event		= get_events(&fdate->event,
				     farr_table,
				     farr_size,
				     fdate->day,
				     fdate->month);

   return(error_event);
}


/* Wrapper function to do a division and a floor call */
float
divf(float x,
     float y)
{
   return( floor(x / y) );
}

/* Determine if Hijri passed-in year is a leap year */
int
h_leapyear(int year)
{
   /* True if year is an Islamic leap year */

   if ( abs(((11 * year) + 14) % 30) < 11 )
      return(1);
   else
      return(0);
}


/* Determine the number of days in passed-in hijri month/year */
int
h_numdays(int month,
	  int year)
{
   /* Last day in month during year on the Islamic calendar. */

   if (((month % 2) == 1) || ((month == 12) && h_leapyear(year)))
      return(30);
   else
      return(29);
}


/* Determine Hijri absolute date from passed-in day/month/year */
int
h_absolute(int day,
	   int month,
	   int year)
{
   /* Computes the Islamic date from the absolute date. */
   return(day				/* days so far this month */
	  + (29 * (month - 1))		/* days so far... */
	  + divf(month, 2)		/* ...this year */
	  + (354 * (year - 1))		/* non-leap days in prior years */
	  + divf((3 + (11 * year)), 30)	/* leap days in prior years */
	  + HijriEpoch - 1);		/* days before start of calendar */
}


/* Determine Hijri/Islamic date from passed-in Gregorian day/month/year
   ie. Gregorian -> Hijri
 */
int
h_date(sDate *cdate,
       int day,
       int month,
       int year)
{
   int abs_date;
   int pre_epoch = 0;
   int error_fill;

   /* Account for Pre-Epoch date correction, year 0 entry */
   if (year < 0)
      year++;

   abs_date = g_absolute(day, month, year);

   /* Search forward/backward year by year from approximate year */
   if (abs_date < HijriEpoch)
   {
      cdate->year = 0;

      while (abs_date <= h_absolute(1, 1, cdate->year))
	 cdate->year--;
   }
   else
   {
      cdate->year = divf((abs_date - HijriEpoch - 1), 355);

      while (abs_date >= h_absolute(1, 1, cdate->year+1))
	 cdate->year++;
   }

   /* Search forward month by month from Muharram */
   cdate->month = 1;
   while (abs_date > h_absolute(h_numdays(cdate->month, cdate->year),
				cdate->month,
				cdate->year))
      cdate->month++;

   cdate->day = abs_date - h_absolute(1, cdate->month, cdate->year) + 1;

   /* Account for Pre-Hijrah date correction, year 0 entry */
   if (cdate->year <= 0)
   {
      pre_epoch   = 1;
      cdate->year = ((cdate->year - 1) * -1);
   }

   /* Set resulting values */
   cdate->units         = ( pre_epoch ? "B.H" : "A.H" );
   cdate->weekday	= (abs(abs_date % 7));
   cdate->frm_numdays	= g_numdays(month, year);
   cdate->to_numdays	= h_numdays(cdate->month, cdate->year);
   cdate->to_numdays2	= h_numdays((cdate->month + 1), cdate->year);

   /* Fill-in the structure with various nicities a user might need */
   error_fill = fill_datestruct(cdate, cdate->weekday, month, cdate->month,
				g_day, g_day_short, g_month, g_month_short,
				h_day, h_day_short, h_month, h_month_short,
				h_events_table, sizeof(h_events_table));

   return(error_fill);
}


/* Determine the number of days in passed-in gregorian month/year */
int
g_numdays(int month,
	  int year)
{
   int y;

   y = abs(year);

   /* Compute the last date of the month for the Gregorian calendar. */
   switch (month)
   {
      case 2:
	 if ( (((y % 4) == 0) && ((y % 100) != 0)) || ((y % 400) == 0) )
	    return(29);
	 else
	    return(28);
      case 4:
      case 6:
      case 9:
      case 11: return(30);
      default: return(31);
   }
}


/* Determine Gregorian absolute date from passed-in day/month/year */
int
g_absolute(int day,
	   int month,
	   int year)
{
   int N = day;           /* days this month */
   int m;

   for (m = month - 1; m > 0; m--) /* days in prior months this year */
      N += g_numdays(m, year);

   return(N				/* days this year */
	  + 365 * (year - 1)		/* previous years days ignoring leap */
	  + divf((year - 1), 4)		/* Julian leap days before this year.. */
	  - divf((year - 1), 100)	/* ..minus prior century years... */
	  + divf((year - 1), 400));	/* ..plus prior years divisible by 400 */
}


/* Determine Gregorian date from passed-in Hijri/Islamic day/month/year
   ie. Hijri -> Gregorian
 */
int
g_date(sDate *cdate,
       int day,
       int month,
       int year)
{
   int abs_date;
   int pre_epoch = 0;
   int error_fill;

   /* Account for Pre-Epoch date correction, year 0 entry */
   if (year < 0)
      year++;

   abs_date = h_absolute(day, month, year);

   /* Search forward year by year from approximate year */
   cdate->year = divf(abs_date, 366);

   while (abs_date >= g_absolute(1, 1, cdate->year+1))
      cdate->year++;

   /* Search forward month by month from January */
   cdate->month = 1;
   while (abs_date > g_absolute(g_numdays(cdate->month, cdate->year),
				cdate->month,
				cdate->year))
      cdate->month++;

   cdate->day = abs_date - g_absolute(1, cdate->month, cdate->year) + 1;

   /* Account for Pre-Hijrah date correction, year 0 entry */
   if (cdate->year <= 0)
   {
      pre_epoch   = 1;
      cdate->year = ((cdate->year - 1) * -1);
   }

   /* Set resulting values */
   cdate->units         = ( pre_epoch ? "B.C" : "A.D" );
   cdate->weekday	= (abs(abs_date % 7));
   cdate->frm_numdays	= h_numdays(month, year);
   cdate->to_numdays	= g_numdays(cdate->month, cdate->year);
   cdate->to_numdays2	= g_numdays((cdate->month + 1), cdate->year);

   /* Place holder for if/when a gregorian table is added */
   /* Fill-in the structure with various nicities a user might need */
   error_fill = fill_datestruct(cdate, cdate->weekday, month, cdate->month,
				h_day, h_day_short, h_month, h_month_short,
				g_day, g_day_short, g_month, g_month_short,
				NULL, 0);

   return(error_fill);
}
