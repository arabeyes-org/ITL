/************************************************************************
 *  Copyright (c) 2004, Arabeyes, Fayez Alhargan
 *
 *    This is a program that computes the Hijary dates for Umm-AlQura
 *    calendar the official calendar of the Kingdom of Saudi Arabia.
 *
 *      King Abdulaziz City for Science and Technology
 *      Computer and Electronics Research Institute
 *      Riyadh, Saudi Arabia
 *      alhargan-at-kacst.edu.sa
 *      Tel:4813770 Fax:4813764
 *
 *    Fayez Alhargan version: opn1.2
 *    Fayez Alhargan last modified 22-1-2003
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "hijri.h"

#define HStartYear 1420
#define HEndYear 1450

extern char *g_day[7];
extern char *h_day[7];
extern char *g_day_short[7];
extern char *h_day_short[7];
extern char *g_month[13];
extern char *h_month[13];
extern char *g_month_short[13];
extern char *h_month_short[13];

int MonthMap[]={19410,
                19396,19337,19093,13613,13741,15210,18132,19913,19858,19110,
                18774,12974,13677,13162,15189,19114,14669,13469,14685,12986,
                13749,17834,15701,19098,14638,12910,13661,15066,18132,18085
               };

/* makes it circular m[0]=m[12] & m[13]=m[1] */
short gmonth[14]={31,31,28,31,30,31,30,31,31,30,31,30,31,31};

/* makes it circular m[0]=m[12] & m[13]=m[1]  */
short smonth[14]={31,30,30,30,30,30,29,31,31,31,31,31,31,30};


/* Various Prototypes */
int  G2H(sDate *mydate, int dg, int mg, int yg);
int  H2G(sDate *mydate, int dh, int mh, int yh);
int  Hsub2G(sDate *mydate, int mh, int yh);

double GCalendarToJD(int yg, int mg, double dg );
double JDToGCalendar(double JD, sDate *mydate);
int GLeapYear(int year);

void GDateAjust(sDate *mydate);
int DayWeek(long JulianD);

void JDToHCalendar(double JD, sDate *mydate);
void JDToHACalendar(double JD, int *yh, int *mh, int *dh);
double HCalendarToJD(int yh, int mh, int dh);
double HCalendarToJDA(int yh, int mh, int dh);
int HMonthLength(int yh, int mh);

double ip(double x);
int mod(double x, double y);
int IsValid(int yh, int mh, int dh);

int fill_datestruct(sDate *fdate, int weekday, int frm_month_num,
		    int to_month_num, char *frm_day[], char *frm_day_short[],
		    char *frm_month[], char *frm_month_short[], char *to_day[],
		    char *to_day_short[], char *to_month[],
		    char *to_month_short[], sEvent *farr_table, int farr_size);

/****************************************************************************/
/* Name:    Hsub2G                                                          */
/* Type:    Procedure                                                       */
/* Purpose: Finds Gdate(year,month,day) for Hdate(year,month,day)           */
/* Arguments:                                                               */
/* Input: Hijri date: year:yh, month:mh                                     */
/* Output: Gregorian date: year:yg, month:mg, day:dg , day of week:dayweek  */
/*         and returns flag found:1 not found:0                             */
/****************************************************************************/
int Hsub2G(sDate *mydate, int mh, int yh)
{
   int flag;
   long JD;
   double GJD;

   /* Make sure that the date is within the range of the tables */
   if(mh < 1) {mh = 12;}
   if(mh > 12) {mh = 1;}
   if(yh < HStartYear) {yh = HStartYear;}
   if(yh > HEndYear)   {yh = HEndYear;}

   GJD = HCalendarToJDA(yh, mh, 1);
   JDToGCalendar(GJD, mydate);
   JD = (long) GJD;
   mydate->weekday = (JD + 1) % 7;
   /* NOTE: so when is flag = 0 ? */
   flag = 1; /* date has been found */

   return(flag);
}

/****************************************************************************/
/* Name:    HCalendarToJDA                                                  */
/* Type:    Function                                                        */
/* Purpose: convert Hdate(year,month,day) to Exact Julian Day               */
/* Arguments:                                                               */
/* Input : Hijrah  date: year:yh, month:mh, day:dh                          */
/* Output:  The Exact Julian Day: JD                                        */
/****************************************************************************/
double HCalendarToJDA(int yh, int mh, int dh)
{
   int flag, Dy, m, b;
   long JD;
   double GJD;

   /* estimate JD of the beginning of year */
   JD = (long) HCalendarToJD(yh, 1, 1);
   Dy = MonthMap[yh-HStartYear]/4096;  /* Mask 1111000000000000 */
   GJD = JD - 3 + Dy;   /* correct the JD value from stored tables  */
   b = MonthMap[yh - HStartYear];
   b = b - Dy * 4096;
   for(m=1; m < mh; m++)
   {
      flag = b % 2;  /* Mask for the current month */
      if(flag)
         Dy = 30;
      else
         Dy = 29;
      GJD = GJD + Dy;   /* Add the months lengths before mh */
      b = (b - flag) / 2;
   }
   GJD = GJD + dh - 1;

   return(GJD);
}

/****************************************************************************/
/* Name:    HMonthLength                                                    */
/* Type:    Function                                                        */
/* Purpose: Obtains the month length                                        */
/* Arguments:                                                               */
/* Input : Hijrah  date: year:yh, month:mh                                  */
/* Output:  Month Length                                                    */
/****************************************************************************/
int HMonthLength(int yh, int mh)
{
   int flag, Dy, m, b;

   if(yh<HStartYear || yh>HEndYear)
   {
      flag = 0;
      Dy = 0;
   }
   else
   {
      Dy = MonthMap[yh - HStartYear]/4096;  /* Mask 1111000000000000 */
      b = MonthMap[yh - HStartYear];
      b = b - Dy * 4096;
      for(m=1; m <= mh; m++)
      {
         flag = b % 2;  /* Mask for the current month */
         if(flag)
            Dy = 30;
         else
            Dy = 29;
         b = (b - flag) / 2;
      }
   }
   return(Dy);
}

/****************************************************************************/
/* Name:    DayInYear                                                       */
/* Type:    Function                                                        */
/* Purpose: Obtains the day number in the yea                               */
/* Arguments:                                                               */
/* Input : Hijrah  date: year:yh, month:mh  day:dh                          */
/* Output:  Day number in the Year                                          */
/****************************************************************************/
int DayinYear(int yh, int mh, int dh)
{
   int flag, Dy, m, b, DL;

   if(yh<HStartYear || yh>HEndYear)
   {
      flag = 0;
      DL = 0;
   }
   else
   {
      Dy = MonthMap[yh - HStartYear]/4096;  /* Mask 1111000000000000 */
      b = MonthMap[yh - HStartYear];
      b = b - Dy * 4096;
      DL = 0;
      for(m = 1; m <= mh; m++)
      {
         flag = b % 2;  /* Mask for the current month */
         if(flag)
            Dy = 30;
         else
            Dy = 29;
         b = (b - flag) / 2;
         DL = DL + Dy;
      }
      DL = DL + dh;
   }
   return(DL);
}

/****************************************************************************/
/* Name:    HYearLength                                                     */
/* Type:    Function                                                        */
/* Purpose: Obtains the year length                                         */
/* Arguments:                                                               */
/* Input : Hijrah  date: year:yh                                            */
/* Output:  Year Length                                                     */
/****************************************************************************/
int HYearLength(int yh)
{
   int flag, Dy, m, b, YL;

   if(yh<HStartYear || yh>HEndYear)
   {
      flag = 0;
      YL = 0;
   }
   else
   {
      Dy = MonthMap[yh - HStartYear]/4096;  /* Mask 1111000000000000 */
      b = MonthMap[yh - HStartYear];
      b = b - Dy * 4096;
      flag = b % 2;  /* Mask for the current month */
      if(flag)
         YL = 30;
      else
         YL = 29;

      for(m = 2; m <= 12; m++)
      {
         flag = b % 2;  /* Mask for the current month */
         if(flag)
            Dy = 30;
         else
            Dy = 29;
         b = (b - flag) / 2;
         YL = YL + Dy;
      }
   }
   return(YL);
}

/****************************************************************************/
/* Name:    G2H                                                             */
/* Type:    Procedure                                                       */
/* Purpose: convert Gdate(year,month,day) to Hdate(year,month,day)          */
/* Arguments:                                                               */
/* Input: Gregorian date: year:yg, month:mg, day:dg                         */
/* Output: Hijrah  date: year:yh, month:mh, day:dh, day of week:dayweek     */
/*       and returns flag found:1 not found:0                               */
/****************************************************************************/
int G2H(sDate *mydate, int dg, int mg, int yg)
{
   int  yh2, mh2;
   int  df;
   int flag = 1;
   long J;
   double GJD, HJD;
   sDate tmpdate;
   sDate tmpdate2;
   /*int error_fill;*/

   GJD = GCalendarToJD(yg, mg, dg + 0.5);  /* find JD of Gdate */

   /* estimate the Hdate that correspond to the Gdate */
   JDToHCalendar(GJD, &tmpdate);  

   /* get the exact Julian Day */
   HJD = HCalendarToJDA(tmpdate.year, tmpdate.month, tmpdate.day);
   df = (int) (GJD - HJD);
   tmpdate.day += df;
   while(tmpdate.day > 30)
   {
      tmpdate.day -= HMonthLength(tmpdate.year, tmpdate.month);
      tmpdate.month++;
      if(tmpdate.month > 12)
      {
         tmpdate.year++;
         tmpdate.month = 1;
      }
   }
   if(tmpdate.day == 30)
   {
      yh2 = tmpdate.year;
      mh2 = tmpdate.month + 1;
      if(mh2 > 12)
      {
         yh2++;
         mh2 = 1;
      }
      Hsub2G(&tmpdate2, mh2, yh2);

      /* Make sure that the month is 30days if not make adjustment */
      if(dg == tmpdate2.day)
      {
	 tmpdate.year = yh2;
	 tmpdate.month = mh2;
	 tmpdate.day = 1;
      }
   }

   J = (long) (GCalendarToJD(yg, mg, dg)+2);
   mydate->weekday = J % 7;
   mydate->to_numdays = 1;	/* this needs to be fixed !! */
   mydate->year = tmpdate.year;
   mydate->month = tmpdate.month;
   mydate->day = tmpdate.day;

   /* Fill-in the structure with various nicities a user might need */
   fill_datestruct(mydate, mydate->weekday, mg, mydate->month,
				g_day, g_day_short, g_month, g_month_short,
				h_day, h_day_short, h_month, h_month_short,
				NULL, 0);

   return(flag);
}

/****************************************************************************/
/* Name:    H2G                                                             */
/* Type:    Procedure                                                       */
/* Purpose: convert Hdate(year,month,day) to Gdate(year,month,day)          */
/* Arguments:                                                               */
/* Input/Ouput: Hijrah  date: year:yh, month:mh, day:dh                     */
/* Output: Gregorian date: year:yg, month:mg, day:dg , day of week:dayweek  */
/*       and returns flag found:1 not found:0                               */
/* Note: The function will correct Hdate if day=30 and the month is 29 only */
/****************************************************************************/
int H2G(sDate *mydate, int dh, int mh, int yh)
{
   int found, yh1, mh1;
   /*int error_fill;*/
   sDate tmpdate;

   /* make sure values are within the allowed values */
   if(dh > 30) { dh = 1;  mh++; }
   if(dh < 1)  { dh = 1;  mh--; }
   if(mh > 12) { mh = 1;  yh++; }
   if(mh < 1)  { mh = 12; yh--; }

   /* find the date of the begining of the month */
   found = Hsub2G(mydate, mh, yh);
   mydate->day += dh - 1;

   /* Make sure that dates are within the correct values */
   GDateAjust(mydate);
   mydate->weekday += dh - 1;
   mydate->weekday = mydate->weekday % 7;

   /*find the date of the begining of the next month*/
   if(dh == 30)
   {
      mh1 = mh + 1;
      yh1 = yh;
      if(mh1 > 12) {mh1 -= 12; yh1++;}
      found = Hsub2G(&tmpdate, mh1, yh1);
      /* Make sure that the month is 30days if not make adjustment */
      if(mydate->day == tmpdate.day)
      {
	 mydate->year = tmpdate.year;
	 mydate->month = tmpdate.month;
	 mydate->day = 1;
      }
   }

   /* Fill-in the structure with various nicities a user might need */
   fill_datestruct(mydate, mydate->weekday, mh, mydate->month,
				h_day, h_day_short, h_month, h_month_short,
				g_day, g_day_short, g_month, g_month_short,
				NULL, 0);

   return(found);
}

/****************************************************************************/
/* Name:    JDToGCalendar                                                   */
/* Type:    Procedure                                                       */
/* Purpose: convert Julian Day  to Gdate(year,month,day)                    */
/* Arguments:                                                               */
/* Input:  The Julian Day: JD                                               */
/* Output: Gregorian date: year:yy, month:mm, day:dd                        */
/****************************************************************************/
double JDToGCalendar(double JD, sDate *mydate)
{
   double A, B, F;
   int alpha, C, E;
   long D, Z;

   Z = (long)floor (JD + 0.5);
   F = (JD + 0.5) - Z;
   alpha = (int)((Z - 1867216.25) / 36524.25);
   A = Z + 1 + alpha - alpha / 4;
   B = A + 1524;
   C = (int) ((B - 122.1) / 365.25);
   D = (long) (365.25 * C);
   E = (int)(((B - D) / 30.6001));
   mydate->day =(int) (B - D - floor (30.6001 * E) + F);
   if (E < 14)
      mydate->month = E - 1;
   else
      mydate->month = E - 13;
   if (mydate->month > 2)
      mydate->year = C - 4716;
   else
      mydate->year = C - 4715;

   F = F * 24.0;

   return(F);
}

/****************************************************************************/
/* Name:    GCalendarToJD                                                   */
/* Type:    Function                                                        */
/* Purpose: convert Gdate(year,month,day) to Julian Day                     */
/* Arguments:                                                               */
/* Input : Gregorian date: year:yy, month:mm, day:dd                        */
/* Output:  The Julian Day: JD                                              */
/****************************************************************************/
double GCalendarToJD(int yy, int mm, double dd)
{
   /* It does not take care of 1582 correction assumes correct
      calender from the past
    */
   int A, B, m, y;
   double T1, T2, Tr;

   if (mm > 2)
   {
      y = yy;
      m = mm;
   }
   else
   {
      y = yy - 1;
      m = mm + 12;
   }
   A = y / 100;
   B = 2 - A + A / 4;
   T1=ip (365.25 * (y + 4716));
   T2=ip (30.6001 * (m + 1));
   Tr=T1+ T2 + dd + B - 1524.5 ;

   return(Tr);
}

/****************************************************************************/
/* Name:    GLeapYear                                                       */
/* Type:    Function                                                        */
/* Purpose: Determines if  Gdate(year) is leap or not                       */
/* Arguments:                                                               */
/* Input : Gregorian date: year                                             */
/* Output:  0:year not leap   1:year is leap                                */
/****************************************************************************/
int GLeapYear(int year)
{
   int T = 0;

   if(year % 4 == 0)
      T = 1;	/* leap_year = 1; */

   if(year % 100 == 0)
   {
      /* years=100,200,300,500,... are not leap years */
      T=0;

      /* years=400,800,1200,1600,2000,2400 are leap years */
      if(year % 400 == 0) T = 1;
   }

   return(T);
}

/****************************************************************************/
/* Name:    GDateAjust                                                      */
/* Type:    Procedure                                                       */
/* Purpose: Adjust the G Dates by making sure that the month lengths        */
/*          are correct if not so take the extra days to next month or year */
/* Arguments:                                                               */
/* Input: Gregorian date: year:yg, month:mg, day:dg                         */
/* Output: corrected Gregorian date: year:yg, month:mg, day:dg              */
/****************************************************************************/
void GDateAjust(sDate *mydate)
{
   int dys;

   /* Make sure that dates are within the correct values */
   /*  Underflow  */
   if(mydate->month < 1)  /* months underflow */
   {
      /* plus as the underflow months is negative */
      mydate->month += 12;
      mydate->year--;
   }

   if(mydate->day < 1)  /* days underflow */
   {
      /* month becomes the previous month */
      mydate->month--;
      /* number of days of the month less the underflow days
         (it is plus as the sign of the day is negative)
       */
      mydate->day += gmonth[mydate->month];
      if(mydate->month == 2)
         mydate->day += GLeapYear(mydate->year);
      if(mydate->month < 1)  /* months underflow */
      {
	 /* plus as the underflow months is negative */
         mydate->month += 12;
         mydate->year--;
      }
   }

   /* Overflow  */
   if(mydate->month > 12)  /* months */
   {
      mydate->month -= 12;
      mydate->year++;
   }

   if(mydate->month == 2)
      /* number of days in the current month */
      dys = gmonth[mydate->month] + GLeapYear(mydate->year);
   else
      dys = gmonth[mydate->month];
   if(mydate->day > dys)  /* days overflow */
   {
      mydate->day -= dys;
      mydate->month++;
      if(mydate->month == 2)
      {
         /* number of days in the current month */
         dys = gmonth[mydate->month] + GLeapYear(mydate->year);
         if(mydate->day > dys)
         {
            mydate->day -= dys;
            mydate->month++;
         }
      }
      if(mydate->month > 12)  /* months */
      {
         mydate->month -= 12;
         mydate->year++;
      }
   }
   mydate->to_numdays = dys;
}

/****************************************************************************/
/*
  The day of the week is obtained as
  Dy=(Julian+1)%7
  Dy=0 Sunday
  Dy=1 Monday
  ...
  Dy=6 Saturday
*/
/****************************************************************************/
int DayWeek(long JulianD)
{
   int Dy;

   Dy = (JulianD + 1) % 7;

   return(Dy);
}

/****************************************************************************/
/* Name:    HCalendarToJD                                                   */
/* Type:    Function                                                        */
/* Purpose: convert Hdate(year,month,day) to estimated Julian Day           */
/* Arguments:                                                               */
/* Input : Hijrah  date: year:yh, month:mh, day:dh                          */
/* Output:  The Estimated Julian Day: JD                                    */
/****************************************************************************/
double HCalendarToJD(int yh, int mh, int dh)
{
   /* Estimating The JD for hijrah dates
      this is an approximate JD for the given hijrah date
    */
   double md, yd;

   md = (mh - 1.0) * 29.530589;
   yd = (yh - 1.0) * 354.367068 + md + dh - 1.0;
   yd = yd + 1948439.0;  /*  add JD for 18/7/622 first Hijrah date */

   return(yd);
}

/****************************************************************************/
/* Name:    JDToHCalendar                                                   */
/* Type:    Procedure                                                       */
/* Purpose: convert Julian Day to estimated Hdate(year,month,day)           */
/* Arguments:                                                               */
/* Input:  The Julian Day: JD                                               */
/* Output : Hijrah date: year:yh, month:mh, day:dh                          */
/****************************************************************************/
void JDToHCalendar(double JD, sDate *mydate)
{
   /* Estimating the hijrah date from JD */
   double md, yd;

   yd = JD-1948439.0;  /*  subtract JD for 18/7/622 first Hijrah date*/
   md = mod(yd, 354.367068);
   mydate->day = mod(md + 0.5, 29.530589)+1;
   mydate->month = (int) ((md/29.530589) + 1);
   yd = yd - md;
   mydate->year = (int) (yd/354.367068 + 1);
   if(mydate->day > 30)   {mydate->day   -= 30; mydate->month++;}
   if(mydate->month > 12) {mydate->month -= 12; mydate->year++;}
}

/****************************************************************************/
/* Name:    JDToHACalendar                                                  */
/* Type:    Procedure                                                       */
/* Purpose: convert Julian Day to  Hdate(year,month,day)                    */
/* Arguments:                                                               */
/* Input:  The Julian Day: JD                                               */
/* Output : Hijrah date: year:yh, month:mh, day:dh                          */
/****************************************************************************/
void JDToHACalendar(double JD, int *yh, int *mh, int *dh)
{
   int df;
   double HJD;
   sDate tmpdate;

   /* Estimate the Hdate that correspond to the Gdate */
   JDToHCalendar(JD, &tmpdate);
   /* get the exact Julian Day */
   HJD  = HCalendarToJDA(tmpdate.year, tmpdate.month, tmpdate.day);
   df   = (int) (JD + 0.5 - HJD);
   tmpdate.day  += df;
   while(tmpdate.day > 30)
   {
      tmpdate.day -= HMonthLength(tmpdate.year, tmpdate.month);
      tmpdate.month++;
      if(tmpdate.month > 12)
      {
	 tmpdate.year++;
	 tmpdate.month = 1;
      }
   }
   if(tmpdate.day == 30 &&
      HMonthLength(tmpdate.year, tmpdate.month) < 30)
   {
      tmpdate.day = 1;
      tmpdate.month++;
   }
   if(tmpdate.month > 12)
   {
      tmpdate.month = 1;
      tmpdate.year++;
   }

/*
   J = JD + 2;
   *dayweek = J % 7;
 */
   *yh = tmpdate.year;
   *mh = tmpdate.month;
   *dh = tmpdate.day;
}

/**************************************************************************/
/* Purpose: return the integral part of a double value.                   */
/**************************************************************************/
double ip(double x)
{
   double  tmp;

   modf(x, &tmp);

   return(tmp);
}

/**************************************************************************/
/* Name:    mod                                                           */
/* Purpose: The mod operation for doubles x mod y                         */
/**************************************************************************/
int mod(double x, double y)
{
   int r;
   double d;

   d = x / y;
   r = (int) d;
   if(r < 0)
      r--;
   d = x - y * r;
   r = (int) d;

   return(r);
}

/**************************************************************************/
/* Purpose: returns 0 for incorrect Hijri date and 1 for correct date     */
/**************************************************************************/
int IsValid(int yh, int mh, int dh)
{
   int valid = 1;

   if((yh < HStartYear) || (yh > HEndYear))
      valid = 0;

   if( (mh < 1) || (mh > 12))
      valid = 0;

   if( (dh < 1) || (dh > HMonthLength(yh, mh)) )
      valid = 0;

   return(valid);
}
