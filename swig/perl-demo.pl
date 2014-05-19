#!/usr/bin/perl
#
#/************************************************************************
# * $Id: perl-demo.pl 10738 2006-09-21 16:39:50Z thamer $
# *
# * ------------
# * Description:
# * ------------
# *  Copyright (c) 2004, Arabeyes, Thamer Mahmoud
# *
# *  This is a test script for using the ITL with SWIG
# *
# *
# * -----------------
# * Revision Details:    (Updated by Revision Control System)
# * -----------------
# *  $Date: 2006-09-21 18:39:50 +0200 (Thu, 21 Sep 2006) $
# *  $Author: thamer $
# *  $Revision: 10738 $
# *  $Source$
# *
# * (www.arabeyes.org - under LGPL license - see COPYING file)
# ************************************************************************/

use libitl_perl;
package libitl_perlc;

## Hijri
$hres = new_sDate();
## Fill the sDate structure
h_date($hres, 24, 10, 2004);
## Display the resutls
print "\n";
print sDate_day_get($hres),"-",sDate_month_get($hres),"-"
    ,sDate_year_get($hres),"\n\n";


## Prayer Times
## Create 6 Prayer objects to be used instead of the array[6]
$prayer0 = new_Prayer();
$prayer1 = new_Prayer();
$prayer2 = new_Prayer();
$prayer3 = new_Prayer();
$prayer4 = new_Prayer();
$prayer5 = new_Prayer();

## Fill the Date structure
$date = new_Date();
Date_day_set($date, 30);
Date_month_set($date, 10);
Date_year_set($date, 1982);
## Fill the Location structure
$loc = new_Location();
Location_degreeLat_set($loc, 24.4833);
Location_degreeLong_set($loc, 54.35);
Location_gmtDiff_set($loc, 4);
Location_dst_set($loc, 0);
Location_seaLevel_set($loc, 0);
Location_pressure_set($loc, 1010);
Location_temperature_set($loc, 10);
## Auto fill the Method structure and set the round value
$conf = new_Method();
getMethod(7, $conf);
Method_round_set($conf, 0);

## From the libitl.i file
getPrayerTimes6($loc, $conf, $date, $prayer0, $prayer1, 
		$prayer2, $prayer3, $prayer4, $prayer5);


## Display the results
print Prayer_hour_get($prayer0),":",Prayer_minute_get($prayer0),":",Prayer_second_get($prayer0),"   ";
print Prayer_hour_get($prayer1),":",Prayer_minute_get($prayer1),":",Prayer_second_get($prayer1),"   ";
print Prayer_hour_get($prayer2),":",Prayer_minute_get($prayer2),":",Prayer_second_get($prayer2),"   ";
print Prayer_hour_get($prayer3),":",Prayer_minute_get($prayer3),":",Prayer_second_get($prayer3),"   ";
print Prayer_hour_get($prayer4),":",Prayer_minute_get($prayer4),":",Prayer_second_get($prayer4),"   ";
print Prayer_hour_get($prayer5),":",Prayer_minute_get($prayer5),":",Prayer_second_get($prayer5),"\n\n";
