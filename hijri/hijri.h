/************************************************************************
 * $Id: hijri.h 12117 2013-09-15 14:22:18Z hosny $
 *
 * ------------
 * Description:
 * ------------
 *  Copyright (c) 2004, Arabeyes, Nadim Shaikli
 *
 *  A Hijri (Islamic) to/from Gregorian (Christian) date conversion library.
 *  This file contains various structures/prototypes to be used within the
 *  library itself.  Some of these structs are of interest to the user others
 *  shouldn't be.
 *
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2013-09-15 16:22:18 +0200 (Sun, 15 Sep 2013) $
 *  $Author: hosny $
 *  $Revision: 12117 $
 *  $Source$
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

#endif  /* _HIJRI_H */
