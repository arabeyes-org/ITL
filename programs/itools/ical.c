/************************************************************************
 * $Id: ical.c 10994 2009-03-09 04:17:36Z thamer $
 *
 * ------------
 * Description:
 * ------------
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  This is an application to primarily display Islamic/Hijri with
 *  association to Gregorian/Meladi dates in 'cal' format (a month
 *  at a time).
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2009-03-09 05:17:36 +0100 (Mon, 09 Mar 2009) $
 *  $Author: thamer $
 *  $Revision: 10994 $
 *  $Source$
 *
 * (www.arabeyes.org - under GPL license)
 ************************************************************************/

/* TODO:
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>             /* for strlen/strcat/etc */

/* For time_t */
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <itl/hijri.h>

#define PROG_NAME	"ical";


/* This holds the current date info. */
typedef struct
{
      int year;
      int month;
      int day;

} Date;


/**
  Print out the command-line usage of this application
 **/
void
usage(int leave)
{
   char *pspaces;
   char *pname = PROG_NAME;

   pspaces = (char *) malloc(strlen(pname));
   strncpy(pspaces, "                      ", strlen(pname));

   fprintf(stderr, "%s [--gregorian yyyymmdd] [--hijri yyyymmdd]\n", pname);
   fprintf(stderr, "%s                        [--umm_alqura]\n", pspaces);
   fprintf(stderr, "%s                        [--fixed_view]\n", pspaces);
   fprintf(stderr, "%s                        [--dual]\n", pspaces);
   fprintf(stderr, "%s                        [--help]\n", pspaces);

   free(pspaces);

   if (leave)
      exit(20);
}


/**
  Error printing (& possibly exit) apparatus.
 **/
void
error(int leave,
      char* err_msg)
{
   char *prog_name = PROG_NAME;

   fprintf(stderr, "[%s]: %s", prog_name, err_msg);

   if (leave)
      exit(leave);
}


/**
  Initizlize various variables/structures
 **/
void
do_init(Date *mydate,
	sDate *pdate)
{
   mydate->year		= (int) NULL;
   mydate->month	= (int) NULL;
   mydate->day		= (int) NULL;

   pdate->day		= (int) NULL;
   pdate->month		= (int) NULL;
   pdate->year		= (int) NULL;
   pdate->weekday	= (int) NULL;
   pdate->frm_numdays	= (int) NULL;
   pdate->to_numdays	= (int) NULL;
   pdate->to_numdays2	= (int) NULL;
   pdate->frm_dname	= (char *) NULL;
   pdate->frm_mname	= (char *) NULL;
   pdate->frm_dname_sh	= (char *) NULL;
   pdate->frm_mname_sh	= (char *) NULL;
   pdate->to_dname	= (char *) NULL;
   pdate->to_mname	= (char *) NULL;
   pdate->to_dname_sh	= (char *) NULL;
   pdate->to_mname_sh	= (char *) NULL;
   pdate->event		= (char **) NULL;
}


/**
  Printout the Resulting date struct along with itsoriginating month view
 **/
void
display_dual(int using_umm_alqura,
	     int doing_hijri,
	     int fixed_view,
	     Date *udate,
	     sDate *rdate)
{
   int i;
   int column;
   int calc_day;
   int end_calc_day;
   int wk_remap[7]	= {1, 2, 3, 4, 5, 6, 0};
   int wk_day;

   if (using_umm_alqura)
      printf("[Using Umm-AlQura Algorithm]\n");

   /* Give us a month, year header */
   printf("From/To (%s %d / %s", rdate->frm_mname,
	  			 udate->year, rdate->to_mname);

   /* Figure out if we are dealing with 2 months on the result or just one */
   if ((rdate->frm_numdays > rdate->to_numdays) ||
       ((rdate->day + rdate->to_numdays - 1) > rdate->frm_numdays))
   {
      printf(" - %s %d", rdate->to_mname2, rdate->year);

      if (rdate->month == 12)
	 printf("-%d):\n", (rdate->year + 1));
      else
	 printf("):\n");
   }
   else
   {
      printf(" %d):\n", rdate->year);
   }

   printf("\n");

   /* Set columns headers */
   if (doing_hijri)
   {
      if (fixed_view)
      {
	 printf("  Sun/   Mon/   Tue/   Wed/   Thu/   Fri/   Sat/\n");
	 printf("  Ahd    Ith    Tha    Arb    Kha    Jum    Sab\n");
      }
      else
      {
	 printf("  Sat/   Sun/   Mon/   Tue/   Wed/   Thu/   Fri/\n");
	 printf("  Sab    Ahd    Ith    Tha    Arb    Kha    Jum\n");
      }
   }
   else
   {
      printf("  Ahd/   Ith/   Tha/   Arb/   Kha/   Jum/   Sab/\n");
      printf("  Sun    Mon    Tue    Wed    Thu    Fri    Sat\n");
   }

   printf("-------------------------------------------------\n");

   /* Do some remapping based on Fri being last day of week in Hijri
      - This is done as a nicety as most hijri calendars end on Friday
    */
   if (doing_hijri & !fixed_view)
      wk_day = wk_remap[rdate->weekday];
   else
      wk_day = rdate->weekday;

   /* Pad with empty space if starting within the week */
   for(i = 0; i < wk_day; i++)
   {
      printf("       ");
   }

   column	= wk_day + 1;
   calc_day	= rdate->day;
   end_calc_day = 0;
   /* Fill in the month view with both FROM and TO values */
   for(i = 1; i <= rdate->frm_numdays; i++)
   {
      if (i == udate->day)
	 printf("[%02d/%02d]", i, calc_day++);
      else
	 printf(" %02d/%02d ", i, calc_day++);

      /* Know when to start a new line */
      if (!(column++ % 7))
	 printf("\n");

      /* Know when to flip the secondary month's day roll-over */
      if (!end_calc_day && (calc_day > rdate->to_numdays))
      {
	 calc_day	= 1;
	 end_calc_day	= 1;
      }

      /* In the VERY unlikely case where the secondary has less days
         than primary such that 3 of the secondary's months are needed,
         know when to start over (yup, sheer paranoia).
       */
      if (end_calc_day && (calc_day > rdate->to_numdays2))
      {
	 calc_day	= 1;
      }
   }

   /* Print a new line if necessary */
   if ((column % 7) != 1)
      printf("\n");
}


/**
  Printout the Resulting date struct in minmal single format (akin to 'cal')
 **/
void
display_single(int using_umm_alqura,
	       int doing_hijri,
	       int fixed_view,
	       Date *udate,
	       sDate *rdate)
{
   int i;
   int shift;
   int column;
   int str_length;
   int dig_length;
   int start_point;
   int calc_day;
   int end_calc_day;
   int wk_remap[7]	= {1, 2, 3, 4, 5, 6, 0};
   int wk_day;

   if (using_umm_alqura)
      printf("[Using Umm-AlQura Algorithm]\n");

   /* Since we get a particular day's info (not day #1), make adjustments */
   shift	= ((rdate->day % 7) - 1);

   /* Do some remapping based on Fri being last day of week in Hijri
      - This is done as a nicety as most hijri calendars end on Friday
    */
   if (doing_hijri & !fixed_view)
      wk_day = wk_remap[rdate->weekday];
   else
      wk_day = rdate->weekday;

   /* Based on day and weekday value of returned result, shift things about */
   if (shift > wk_day)
      column = (wk_day - shift + 7 + 1);
   else
      column = (wk_day - shift + 1);

   /* Catch spill-over condition when dealing with lastday of week */
   column = ( (column == 8) ? 1 : column);

   /* Give us a month, year header (nicely centered) */
   if (rdate->year/10)
      dig_length = 1;
   if (rdate->year/100)
      dig_length += 1;
   if (rdate->year/1000)
      dig_length += 1;

   /* Try to center the title as much as possible over the output */
   str_length	= ( strlen(rdate->to_mname) + dig_length + 6);
   start_point	= ( 14 - (str_length / 2));

   for(i = 1; i < start_point; i++)
      printf(" ");
   printf("%s %d (%s)\n", rdate->to_mname, rdate->year, rdate->units);

   /* Set columns headers based on what we're converting to */
   if (doing_hijri)
      if (fixed_view)
      {
	 printf(" Ah   I   T  Ar   K   J   S\n");
      }
      else
      {
	 printf("  S  Ah   I   T  Ar   K   J\n");
      }
   else
      printf("  S   M  Tu   W  Th   F   S\n");

   /* Pad with empty space if starting within the week */
   for(i = 0; i < column-1; i++)
   {
      printf("    ");
   }

   calc_day	= rdate->day;
   end_calc_day = 0;
   /* Fill in the month view with both FROM and TO values */
   for(i = 1; i <= rdate->to_numdays; i++)
   {
      if (i == rdate->day)
	 printf("[%2d]", i);
      else
	 printf(" %2d ", i);

      /* Know when to start a new line */
      if (!(column++ % 7))
	 printf("\n");

      /* Know when to flip the secondary month's day roll-over */
      if (!end_calc_day && (calc_day > rdate->to_numdays))
      {
	 calc_day	= 1;
	 end_calc_day	= 1;
      }

      /* In the very unlikely case were the secondary has less days
         than primary such that 3 of the secondary's months are needed,
         know when to start over
       */
      if (end_calc_day && (calc_day > rdate->to_numdays2))
      {
	 calc_day	= 1;
      }
   }

   /* Print a new line if necessary */
   if ((column % 7) != 1)
      printf("\n");
}


/**
  Main procedure
 **/
int main(int argc, char *argv[])
{
   int i;
   int g_to_h		= 0;
   int h_to_g		= 0;
   int use_umm_alqura	= 0;
   int show_dual	= 0;
   int fixed_week_view	= 0;
   int error_lib	= 0;
   int start_day;
   Date indate;
   sDate outdate;

   /* Current time/date specifics */
   time_t mytime;
   struct tm *t_ptr;

   do_init(&indate, &outdate);

   /* Process the command-line */
   for (i = 1; i < argc; i++)
   {
     if (strcasecmp(argv[i], "-h") == 0 ||
         strcasecmp(argv[i], "-help") == 0 ||
         strcasecmp(argv[i], "--help") == 0)
     {
        /* We really need a full-fledged help here */
        usage(1);
     }

     if (strcasecmp(argv[i], "-g") == 0 ||
         strcasecmp(argv[i], "-gregorian") == 0 ||
         strcasecmp(argv[i], "--gregorian") == 0)
     {
	g_to_h = 1;
        if (argv[i+1] != NULL)
        {
           sscanf(&(argv[i + 1][0]), "%4d", &indate.year);
           sscanf(&(argv[i + 1][4]), "%2d", &indate.month);
           sscanf(&(argv[i + 1][6]), "%2d", &indate.day);
        }
        else
        {
           error(1, "Exiting, invalid argument to --gregorian\n");
        }
     }

     if (strcasecmp(argv[i], "-hi") == 0 ||
         strcasecmp(argv[i], "-hijri") == 0 ||
         strcasecmp(argv[i], "--hijri") == 0)
     {
	h_to_g = 1;
        if (argv[i+1] != NULL)
        {
           sscanf(&(argv[i + 1][0]), "%4d", &indate.year);
           sscanf(&(argv[i + 1][4]), "%2d", &indate.month);
           sscanf(&(argv[i + 1][6]), "%2d", &indate.day);
        }
        else
        {
           error(1, "Exiting, invalid argument to --hijri\n");
        }
     }

     if (strcasecmp(argv[i], "-u") == 0 ||
         strcasecmp(argv[i], "-umm_alqura") == 0 ||
         strcasecmp(argv[i], "--umm_alqura") == 0)
     {
	 use_umm_alqura = 1;
     }

     if (strcasecmp(argv[i], "-f") == 0 ||
         strcasecmp(argv[i], "--fixed_view") == 0)
     {
	 fixed_week_view = 1;
     }

     if (strcasecmp(argv[i], "-d") == 0 ||
         strcasecmp(argv[i], "--dual") == 0)
     {
	 show_dual = 1;
     }
   }

   /* Make sure user knows what they are doing */
   if (g_to_h && h_to_g)
      error(2, "Exiting, can't defined both Gregorian and Hijri\n");

   /* Go with default current date if not user specifications */
   if (!g_to_h && !h_to_g)
   {
      g_to_h = 1;

      /* Get current time structure */
      time(&mytime);

      t_ptr = localtime(&mytime);

      /* Set current time values */
      indate.day   = t_ptr->tm_mday;
      indate.month = t_ptr->tm_mon  + 1;
      indate.year  = t_ptr->tm_year + 1900;
   }

   /* Inspect user's input or default settings - within range ? */
   if ((indate.month > 12) || (indate.month < 1))
      error(3, "Exiting, input 'month' not within allowable range\n");

   if ((indate.day > 31) || (indate.day < 1))
      error(3, "Exiting, input 'day' not within allowable range\n");

   /* Specify our appropriate start day based on user mode */
   start_day = ( show_dual ? 1 : indate.day );

   /* TEMPORARY note due to no functionality */
   if (use_umm_alqura)
      printf("NOTE: Umm Al-Qura has NOT been included YET (using other)\n");

   /* Do the actual conversions */
   if (g_to_h)
   {
      /* Dealing with Gregorian to Hijri conversion */
/*
      if (use_umm_alqura)
	 G2H(&outdate, indate.day, indate.month, indate.year);
      else
 */
	 error_lib = h_date(&outdate, start_day, indate.month, indate.year);
   }
   else
   {
      /* Dealing with Hijri to Gregorian conversion */
/*
      if (use_umm_alqura)
	 H2G(&outdate, indate.day, indate.month, indate.year);
      else
 */
	 error_lib = g_date(&outdate, start_day, indate.month, indate.year);
   }

   /* Let user know if the library freaked-out and returned with an error */
   if (error_lib)
   {
      char msg[80];

      sprintf(msg, "Exiting, error code %d generated by library call\n",
	      error_lib);
      error(error_lib, msg);
   }

   /* Spill-out and display the output results */
   if (show_dual)
//      display_dual(use_umm_alqura, g_to_h, &indate, &outdate);
      display_dual(0, g_to_h, fixed_week_view, &indate, &outdate);
   else
//      display_single(use_umm_alqura, g_to_h, &indate, &outdate);
      display_single(0, g_to_h, fixed_week_view, &indate, &outdate);

   return(0);
}
