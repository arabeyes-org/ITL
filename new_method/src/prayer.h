/*
 Islamic Prayer Times and Qibla Direction Library
 By Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)
 Licensed under the BSD license shown in file LICENSE

 This is a "clean-room" implementation that uses simpler but
 approximate methods. This implementation should be easier
 to maintain and debug. However, we need to first to test it
 throughly before replacing the old method. Currently, it
 supports calculating the prayer times and qibla location for
 "normal" latitudes. However, "extreme latitude" methods are
 still missing.
*/

#ifndef _PRAYER_H_
#define _PRAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "defs.h"

extern calc_method_t calc_methods [];

/* Public functions */

void get_prayer_times(const struct tm *day,
                      const struct location *loc,
                      struct prayer_times *pt);

double get_qibla_direction(const struct location *loc);

#endif
