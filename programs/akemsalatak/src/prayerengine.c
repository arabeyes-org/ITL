/* prayerEngine.c */

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


#include "prayerEngine.h"


/* astro stuff */
static double EquationOfTime(int nDay, double lastday);
static double DeclinationOfSun(int nDay);
/*  Double Base-10 to int Base-6 */
static void Base6HM(double sTime, int *hour, int *minute, int DST);
static double Deg2Rad(double n);
static double GetThuhr(double Lon, double Ref, double EOT);
static double GetShoMag(double Lat, double DEC, double SeaLev);
static double GetFajIsh(double Lat, double DEC, double Ang);
static double GetAssr(double Lat, double DEC, int Mathhab);



void GetPrayerTimes( sLocation loc, sMethod conf, sDate date, 
                     sPrayerTimes *pt)
{

  int i, nDay, lastday, nNextDay;
  double EOT, DEC, Th, ShMg, Fj, Is, Ar, Im;
  double NextEOT, NextDEC, NextTh, NextShMg, NextFj, NextIm;
    
  double tempPrayer[8];
  double Lat = loc.DegreeLat; 
  double Lon = loc.DegreeLong; 
  double Ref = loc.GMTdiff * 15; 
  nDay = GetDayofYear(date.year, date.month, date.day);
  lastday = GetDayofYear(date.year, 12, 31);
    

  /* 
     First Step: Get Prayer Times Equations' results for this day of year 
     and this location. The results are NOT the actual prayer times
  */
  EOT = EquationOfTime(nDay, lastday);
  DEC = DeclinationOfSun(nDay);
  Th = GetThuhr(Lon, Ref, EOT);
  ShMg = GetShoMag(Lat, DEC, loc.SeaLev);
  Fj = GetFajIsh(Lat, DEC, conf.FajrAng);
  Im = GetFajIsh(Lat, DEC, conf.FajrAng +  conf.ImsaakAng);
  Is = GetFajIsh(Lat, DEC, conf.IshaaAng);
  Ar = GetAssr(Lat, DEC, conf.Mathhab);


  /* Next Day Fajr and Imsaak*/
  nNextDay = nDay + 1;
  if(nNextDay > lastday)
    nNextDay=1;
  NextEOT = EquationOfTime(nNextDay, lastday);
  NextDEC = DeclinationOfSun(nNextDay);
  NextTh = GetThuhr(Lon, Ref, NextEOT);
  NextShMg = GetShoMag(Lat, NextDEC, loc.SeaLev);
  NextFj = GetFajIsh(Lat, NextDEC, conf.FajrAng);
  NextIm = GetFajIsh(Lat, NextDEC, conf.FajrAng + conf.ImsaakAng);  

    
  /* 
     Second Step A: Calculate all salat times as Base-10 numbers in 
     Normal circumstances
  */
  /* Fajr */
  if (conf.FajrINV != 0) {
    double INV = conf.FajrINV / 60.0;
    tempPrayer[0] = Th - ShMg - INV;
    tempPrayer[6] = NextTh - NextShMg - INV;
    tempPrayer[7] = NextTh - NextShMg - INV - (conf.ImsaakINV /60.0);
        
  } else if (Fj == 0) {
    tempPrayer[0] = 0;
    tempPrayer[6] = 0;
    tempPrayer[7] = 0;

  } else {
    tempPrayer[0] = Th - Fj;
    tempPrayer[6] = NextTh - NextFj;
    tempPrayer[7] = NextTh - NextIm;
  }
    
  tempPrayer[1] = Th - ShMg;
  tempPrayer[2] = Th;
  tempPrayer[3] = Th + Ar;
  tempPrayer[4] = Th + ShMg;
    

  /* Ishaa */
  if (Is == 0)
    tempPrayer[5] = 0;
  else tempPrayer[5] = Th + Is; 
  /* Ishaa Interval */
  if (conf.IshaaINV != 0) {
    double INV = conf.IshaaINV / 60.0;
    tempPrayer[5] = Th + ShMg + INV; 
  }
    
  
    
    
  /* 
     Second Step B: Calculate all salat times as Base-10 numbers in 
     Extreme Latitudes (if set)
  */
    
  if (conf.isExtreme != 0)
    {   
        
      double exEOT, exDEC, exTh, exShMg, exFj, exIs, exAr, exIm;
      double exNextFj, exNextDEC, exNextIm;
        
      /* Nearest Latitude (NEAREST_LAT) */
      if (conf.isExtreme <= 3) 
        {
          exFj = GetFajIsh(NEAREST_LAT, DEC, conf.FajrAng);
          exIm = GetFajIsh(NEAREST_LAT, DEC, conf.ImsaakAng);
          exNextFj = GetFajIsh(NEAREST_LAT, NextDEC, conf.FajrAng);
          exNextIm = GetFajIsh(NEAREST_LAT, NextDEC,  conf.FajrAng + conf.ImsaakAng);
          exIs = GetFajIsh(NEAREST_LAT, DEC, conf.IshaaAng);
          exAr = GetAssr(NEAREST_LAT, DEC, conf.Mathhab);
          exShMg = GetShoMag(NEAREST_LAT, DEC, loc.SeaLev);
            
          switch(conf.isExtreme)
            {
            case 1: /* All salat Always: Nearest Latitude */
              tempPrayer[0] = Th - exFj;
              tempPrayer[1] = Th - exShMg;
              tempPrayer[3] = Th + exAr;
              tempPrayer[4] = Th + exShMg;
              tempPrayer[5] = Th + exIs;
              tempPrayer[6] = NextTh - exNextFj;
              tempPrayer[7] = NextTh - exNextIm;
              break;
                
            case 2: /* Fajr Ishaa Always: Nearest Latitude */
              tempPrayer[0] = Th - exFj;
              tempPrayer[5] = Th + exIs;
              tempPrayer[6] = NextTh - exNextFj;
              tempPrayer[7] = NextTh - exNextIm;
              break;
                
            case 3:/* Fajr Ishaa if invalid: Nearest Latitude */
              if (tempPrayer[0] <= 0)
                tempPrayer[0] = Th - exFj;
              if (tempPrayer[5] <= 0)
                tempPrayer[5] = Th + exIs; 
              if (tempPrayer[6] <= 0)
                tempPrayer[6] = NextTh - exNextFj;
              if (tempPrayer[7] <= 0)
                tempPrayer[7] = NextTh - exNextIm;
              break;
            }
        } /* Nearest latitude */
        
        
      /* Nearest Good Day */
      if ((conf.isExtreme > 3) && (conf.isExtreme <= 5)) 
        {
          int nGoodDay = 0;
            
          /* Start by getting last or next nearest Good Day */
          for(i=0; i <= lastday; i++)
            {
              /* last closest day */
              nGoodDay = nDay - i;
              exEOT = EquationOfTime(nGoodDay, lastday);
              exDEC = DeclinationOfSun(nGoodDay);
              exTh = GetThuhr(Lon, Ref, exEOT);
              exFj = GetFajIsh(Lat, exDEC, conf.FajrAng);
              exIs = GetFajIsh(Lat, exDEC, conf.IshaaAng);
                
              if ((exFj > 0) && (exIs > 0))
                break; /* loop */
                
                
              /* Next closest day */
              nGoodDay = nDay + i;
              exDEC = DeclinationOfSun(nGoodDay);
              exTh = GetThuhr(Lon, Ref, exEOT);
              exFj = GetFajIsh(Lat, exDEC, conf.FajrAng);
              exIs = GetFajIsh(Lat, exDEC, conf.IshaaAng);
                
              if ((exFj > 0) && (exIs > 0))
                break; 
            }
            
          exEOT = EquationOfTime(nGoodDay, lastday);
          exDEC = DeclinationOfSun(nGoodDay);
          NextDEC = DeclinationOfSun(nGoodDay);
          exTh = GetThuhr(Lon, Ref, exEOT);
          exFj = GetFajIsh(Lat, exDEC, conf.FajrAng);
          exIs = GetFajIsh(Lat, exDEC, conf.IshaaAng);
          /* XXXThamer: next day is probebly not a good day. */
          exNextDEC = DeclinationOfSun(nGoodDay+1);
          exNextFj = GetFajIsh(Lat, exNextDEC, conf.FajrAng);
          /* XXXThamer: Imsaak angle maybe not a valid angle? */
          exNextIm = GetFajIsh(Lat, exNextDEC,  conf.FajrAng + conf.ImsaakAng);
          exShMg = GetShoMag(Lat, exDEC, loc.SeaLev);
          exAr = GetAssr(Lat, exDEC, conf.Mathhab);
            
          switch(conf.isExtreme)
            {
            case 4: /* All salat Always: Nearest Day */
              tempPrayer[0] = exTh - exFj;
              tempPrayer[1] = exTh - exShMg;
              tempPrayer[3] = exTh + exAr;
              tempPrayer[4] = exTh + exShMg;
              tempPrayer[5] = exTh + exIs;
              tempPrayer[6] = NextTh - exNextFj;
              tempPrayer[7] = NextTh - exNextIm;
              break;
                
            case 5: /* Fajr Ishaa if invalid:: Nearest Day */
              if (tempPrayer[0] <= 0)
                tempPrayer[0] = exTh - exFj;
              if (tempPrayer[5] <= 0)
                tempPrayer[5] = exTh + exIs; 
              if (tempPrayer[6] <= 0)
                tempPrayer[6] = NextTh - exNextFj;
              if (tempPrayer[7] <= 0)
                tempPrayer[7] = NextTh - exNextIm;
              break; 
                
            } /* end switch */
            
            
        } /* end nearest day */
        
      /*1/7th of Night */
      if (conf.isExtreme == 6 || conf.isExtreme == 7) 
        {
          double allInterval = 24 - (Th - ShMg);
          allInterval = allInterval + (12 - (Th + ShMg));
            
          switch(conf.isExtreme)
            {
            case 6: /*Fajr Ishaa Always: 1/7th of Night */
              tempPrayer[0] = (Th - ShMg) - ((1/7.0) * allInterval);
              tempPrayer[5] = ((1/7.0) * allInterval) + (Th + ShMg);
              tempPrayer[6] = (NextTh - NextShMg) - ((1/7.0) * allInterval);
              /* XXXThamer : Imsaak calc here is untestesd */
              tempPrayer[6] = (NextTh - NextShMg) - ((1/7.0) * allInterval)
                - (conf.ImsaakINV /60.0);
              break;
                
            case 7: /*Fajr Ishaa if invalid: 1/7th of Night */
              if (tempPrayer[0] <= 0)
                tempPrayer[0] = (Th - ShMg) - ((1/7.0) * allInterval);
              if (tempPrayer[5] <= 0)
                tempPrayer[5] = ((1/7.0) * allInterval) + (Th + ShMg);
              if (tempPrayer[6] <= 0)
                tempPrayer[6] = (NextTh - NextShMg) - ((1/7.0) * allInterval);
              /* XXXThamer : Imsaak calc here is untestesd */
              if (tempPrayer[7] <= 0)
                tempPrayer[7] = (NextTh - NextShMg) - ((1/7.0) * allInterval)
                  - (conf.ImsaakINV /60.0);
              break;
            }
        } /* end 1/7th of Night */
        
      /* n Minutes from shorooq maghrib */
      if (conf.isExtreme == 8 || conf.isExtreme == 9) 
        {
          switch(conf.isExtreme)
            {
            case 8: /* Minutes from Shorooq/Maghrib Always */
              tempPrayer[0] = Th - ShMg;
              tempPrayer[5] = Th + ShMg;
              tempPrayer[6] = NextTh - NextShMg;
	      tempPrayer[7] = NextTh - NextShMg;
              break;
                
            case 9: /* Minutes from Shorooq/Maghrib if invalid */
              if (tempPrayer[0] <= 0)
                tempPrayer[0] = Th - ShMg;
              if (tempPrayer[5] <= 0)
                tempPrayer[5] = Th + ShMg;
              if (tempPrayer[6] <= 0)
                tempPrayer[6] = NextTh - NextShMg;
              if (tempPrayer[7] <= 0)
                tempPrayer[7] = NextTh - NextShMg;
              break;
                
            } /* end switch */
        } /* end n Minutes */
        
    } /* End Extreme */
    
    
  /* 
     Third and Final Step: Fill the sPrayerTimes structure and 
     do decimal to minutes conversion
  */
  for (i=0; i<8; i++)
    Base6HM(tempPrayer[i], &pt->nPrayerH[i], &pt->nPrayerM[i], loc.DST);
      
}



static void Base6HM(double sTime, int *hour, int *minute, int DST)
{
  double temp = (sTime - floor(sTime)) * 60;
  if (sTime<0)
    sTime *= -1;
  sTime += DST;
  if (sTime > 23)
    sTime = 0;
  *hour = (int)sTime;
  *minute = (int)temp;  
    
}



static double GetThuhr(double Lon, double Ref, double EOT)
{
  return 12 + (Ref-Lon)/15.0 - (EOT/60.0);
}


static double GetShoMag(double Lat, double DEC, double SeaLev)
{
  double part1 = sin(Deg2Rad(Lat)) * sin(DEC);
  double part2 = (sin(Deg2Rad(-0.8333-0.0347*pow(SeaLev,0.5))) - part1);
  double part3 = cos(Deg2Rad(Lat)) * cos(DEC);
    
  return 0.0667 * (acos(part2 / part3) / Deg2Rad(0));
}


static double GetFajIsh(double Lat, double DEC, double Ang)
{
  double part1 = cos(Deg2Rad(Lat)) * cos(DEC);
  double part2 = -sin(Deg2Rad(Ang)) - sin(Deg2Rad(Lat)) * sin(DEC);
    
  double test = part2 / part1;
  if (test <= -1)
    return 0;
    
  return 0.0667 * (acos(part2 / part1) / Deg2Rad(0));
}


static double GetAssr(double Lat, double DEC, int Mathhab)
{
  double part1, part2, part3, part4;
    
  part1 = Mathhab + tan(Deg2Rad(Lat) - DEC);
  if (Lat < 0)
    part1 = Mathhab - tan(Deg2Rad(Lat) - DEC);
    
  part2 = (PI/2.0) - atan(part1);
  part3 = sin(part2) - sin(Deg2Rad(Lat)) * sin(DEC);
  part4 = (part3 / (cos(Deg2Rad(Lat)) * cos(DEC)));
    
  return 0.0667 * acos(part4) / Deg2Rad(0);
}


static double EquationOfTime(int nDay, double lastday)
{
  double EOT;
  double t = 360 * (nDay-81.0) / (double)lastday;
  t = Deg2Rad(t);
  EOT = 9.87 * sin(2*t) - 7.53 * cos(t) - 1.5 * sin(t);
    
  return EOT;
}

static double DeclinationOfSun(int nDay)
{
  double DEC = Deg2Rad(23.45) * sin(Deg2Rad(.9836 * (284 + nDay)));
  return DEC;
    
}


/* Convert Degrees to Radians (and vice-versa if n==0) */
static double Deg2Rad(double n)
{       
  if (n==0)
    return (PI/180.0);
  else return n * (PI/180.0);
}


int GetDayofYear(int year, int month, int day)
{
  int i;
  int isLeap = ((year & 3) == 0) && ((year % 100) != 0 
                                     || (year % 400) == 0);
  
  static char dayList[2][13] = {
    {0,31,28,31,30,31,30,31,31,30,31,30,31},
    {0,31,29,31,30,31,30,31,31,30,31,30,31}
  };

  for (i=1; i<month; i++)
    day += dayList[isLeap][i];
  
    
  return day;
}


double Dms2Decimal(int deg, int min, int sec)
{
  return deg + ((min/60.0)+(sec/3600.0));
}

void Decimal2Dms(double dec, int *n1,int *n2, int *n3)
{
  double tempmin, tempsec, deg, min, sec, test;

  tempmin = modf(dec, &deg) * 60.0;
  tempsec = modf(tempmin, &min) * 60.0;


  /* Rounding seconds */
  if (deg < 0)
    sec = ceil(tempsec);
  else sec = floor(tempsec);

  test = tempsec - sec;

  if(test > 0.5)
    sec++;

  if (test < 0 && test < -0.5)
    sec--;

  if (sec == 60)
    {
      sec = 0;
      min++;
    }

  if (sec == -60)
    {
      sec = 0;
      min--;
    }

  *n1 = (int)deg;
  *n2 = (int)min;
  *n3 = (int)sec;

}
