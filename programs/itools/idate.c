/************************************************************************
 * $Id: idate.c 10994 2009-03-09 04:17:36Z thamer $
 *
 * ------------
 * Description:
 * ------------
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  This is an application to primarily convert the Gregorian/Meladi
 *  date to Islamic/Hijri (from Hijri to Meladi is also offered).
 *  There are two distinct Hijri calculation conversion algorithms
 *  available (the '--umm_alqura' selecting the calculation method
 *  presented and advocated by the Saudi Umm Al-Qura University).
 *  It is envisioned that both the non-umm_alqura and the umm_alqura
 *  options will not always yield the same results.
 *
 *  NOTE: Although the non-umm_alqura algorithm used is dated, it is
 *        at this point the best implementation found.  Work will be
 *        on-going to improve its accuracy and implementation.
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
   - Define error exit codes (standardize 'em) and associate a string
     with each to print out to let user know what went wrong
     So far:
       0 - Normal exit (no errors)
       1 - User entered invalid argument to input
       2 - User entered mututally-exclusive inputs
       3 - User entered dates beyond acceptable ranges
      10 - Can't malloc/attain memory
      20 - User invoked help
   - Add a proper 'man' page and documentation
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

#define PROG_NAME	"idate";


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
   fprintf(stderr, "%s                        [--simple]\n", pspaces);
   fprintf(stderr, "%s                        [--umm_alqura]\n", pspaces);
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
//   pdate->units		= (char *) NULL;
   pdate->units		= "A.H";
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
  Printout the results
 **/
void
display_output(int simple_mode,
	       int using_umm_alqura,
	       Date *indate,
	       sDate *rdate)
{
   int i;

   /* See if we're only to print out a simple output */
   if (simple_mode)
   {
      printf("%2d/%2d/%4d %s", rdate->day,
	     		       rdate->month,
	     		       rdate->year,
	     		       rdate->units);
      printf(" - %s(%s) - %s(%s)\n", rdate->to_dname,
	     			     rdate->to_dname_sh,
	     			     rdate->to_mname,
	     			     rdate->to_mname_sh);
   }
   else
   {
      printf("Date Format (dd/mm/yyyy)");

      if (using_umm_alqura)
	 printf(" [using Umm-AlQura Algorithm]:\n");
      else
	 printf(":\n");

      printf("+ Input    : %2d/%2d/%4d    ", indate->day,
	  			             indate->month,
	  			             indate->year);

      printf("  - %10s(%s) - %12s(%s)\n", rdate->frm_dname,
	  			          rdate->frm_dname_sh,
	  			          rdate->frm_mname,
	  			          rdate->frm_mname_sh);

      printf("-----------------------------\n");
      printf("+ Output   : %2d/%2d/%4d %s", rdate->day,
	  			            rdate->month,
	  				    rdate->year,
      					    rdate->units);

      printf("  - %10s(%s) - %12s(%s)\n", rdate->to_dname,
	  			          rdate->to_dname_sh,
	  			          rdate->to_mname,
	  			          rdate->to_mname_sh);
   }

   /* Print out any event(s) on the specified date */
   if (rdate->event[0])
      printf("\n");

   for (i = 0; rdate->event[i] != NULL; i++)
   {
      printf("(*) Event on this Day : %s\n", rdate->event[i]);
   }
   /* Do some house cleaning - just in case */
   free(rdate->event);
}


/**
  Main procedure
 **/
int main(int argc, char *argv[])
{
   int i;
   int g_to_h		= 0;
   int h_to_g		= 0;
   int simple_mode	= 0;
   int use_umm_alqura	= 0;
   int error_lib	= 0;
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

     if (strcasecmp(argv[i], "-s") == 0 ||
         strcasecmp(argv[i], "-simple") == 0 ||
         strcasecmp(argv[i], "--simple") == 0)
     {
	 simple_mode = 1;
     }

     if (strcasecmp(argv[i], "-u") == 0 ||
         strcasecmp(argv[i], "-umm_alqura") == 0 ||
         strcasecmp(argv[i], "--umm_alqura") == 0)
     {
	 use_umm_alqura = 1;
     }
   }

   /* Make sure user knows what they are doing */
   if (g_to_h && h_to_g)
      error(2, "Exiting, can't define both Gregorian and Hijri\n");

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

   /* Do the actual conversions */
   if (g_to_h)
   {
      /* Dealing with Gregorian to Hijri conversion */
      if (use_umm_alqura)
	 G2H(&outdate, indate.day, indate.month, indate.year);
      else
	 error_lib = h_date(&outdate, indate.day, indate.month, indate.year);
   }
   else
   {
      /* Dealing with Hijri to Gregorian conversion */
      if (use_umm_alqura)
	 H2G(&outdate, indate.day, indate.month, indate.year);
      else
	 error_lib = g_date(&outdate, indate.day, indate.month, indate.year);
   }

   /* Let user know if the library freaked-out and returned with an error */
   if (error_lib)
   {
      char msg[80];

      sprintf(msg, "Exiting, error code %d generated by library call\n",
	      error_lib);
      error(error_lib, msg);
   }

   /* Spill-out the results */
   display_output(simple_mode, use_umm_alqura, &indate, &outdate);

   return(0);
}
