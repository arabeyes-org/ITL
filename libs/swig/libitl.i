/************************************************************************
 * $Id: libitl.i 10738 2006-09-21 16:39:50Z thamer $
 *
 * ------------
 * Description:
 * ------------
 *  Copyright (c) 2004, Arabeyes, Thamer Mahmoud
 *
 *  This an example SWIG interface file for the ITL libaray. 
 *
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2006-09-21 18:39:50 +0200 (Thu, 21 Sep 2006) $
 *  $Author: thamer $
 *  $Revision: 10738 $
 *  $Source$
 *
 * (www.arabeyes.org - under LGPL license - see COPYING file)
 ************************************************************************/

// NOTE: Change the perl prefix to whatever language you want to use
%module libitl_perl
%{
#include "../prayertime/src/prayer.h"
#include "../hijri/src/hijri.h"
%}

%include "../prayertime/src/prayer.h"
%include "../hijri/src/hijri.h"

%inline %{
    // NOTE: In case of trouble with passing C arrays from a script, this is an
    // alternative to the "getPrayerTimes" API function that can take 6 separate
    // Prayer objects instead of array[6].
    void getPrayerTimes6 (const Location* loc, const Method* conf,
                          const Date* date, Prayer* p0,
                          Prayer* p1,  Prayer* p2,  Prayer* p3,
                          Prayer* p4,  Prayer* p5)
        {
            Prayer pList[6];
            getPrayerTimes (loc, conf, date, pList);
            *p0 = pList[0];
            *p1 = pList[1]; 
            *p2 = pList[2];
            *p3 = pList[3];
            *p4 = pList[4];
            *p5 = pList[5]; 
        }
%}
