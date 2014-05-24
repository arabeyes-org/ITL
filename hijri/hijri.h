/************************************************************************
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  A Hijri (Islamic) to/from Gregorian (Christian) date conversion library.
 *  This file contains various structures/prototypes to be used within the
 *  library itself.  Some of these structs are of interest to the user others
 *  shouldn't be.
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

#ifndef _HIJRI_H
#define _HIJRI_H

/* User-viewable Date structure */
typedef struct
{
    int day;		/* Day */
    int month;		/* Month */
    int year;		/* Year */
    int weekday;        /* Day of the week (0:Sunday, 1:Monday...) */
    int frm_numdays;    /* Number of days in specified input  month */
    int to_numdays;     /* Number of days in resulting output month */
    int to_numdays2;    /* Number of days in resulting output month+1 */
    char *units;	/* Units used to denote before/after epoch */
    char *frm_dname;	/* Converting from - Name of day */
    char *frm_mname;	/* Converting from - Name of month */
    char *frm_dname_sh;	/* Converting from - Name of day   in short format */
    char *frm_mname_sh;	/* Converting from - Name of month in short format */
    char *to_dname;	/* Converting to   - Name of day */
    char *to_mname;	/* Converting to   - Name of month */
    char *to_mname2;	/* Converting to   - Name of month+1 */
    char *to_dname_sh;	/* Converting to   - Name of day   in short format */
    char *to_mname_sh;	/* Converting to   - Name of month in short format */
    char **event;	/* Important event pertaining to date at hand */
} sDate;

/* Table populated structure */
typedef struct
{
    int day;
    int month;
    char *event;
} sEvent;

/* Prototypes */

int h_date(sDate *cdate, int day, int month, int year);
int g_date(sDate *cdate, int day, int month, int year);

int G2H(sDate *cdate, int yg, int mg, int dg);
int H2G(sDate *cdate, int yh, int mh, int dh);

int g_absolute(int day, int month, int year);
int g_numdays(int month, int year);

int fill_datestruct(sDate *fdate, int weekday, int frm_month_num,
		int to_month_num, char *frm_day[], char *frm_day_short[],
		char *frm_month[], char *frm_month_short[], char *to_day[],
		char *to_day_short[], char *to_month[], char *to_month_short[],
		sEvent *farr_table, int farr_size);

#endif  /* _HIJRI_H */
