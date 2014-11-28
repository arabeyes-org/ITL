/* Needed to expose POSIX stuff under C89 mode */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prayer.h"
#include "config.h"

int main(int argc, char **argv)
{
    struct location loc;
    struct tm date;
    time_t t;
    double qibla;
    struct prayer_times pt;

    parse_arguments(argc, argv, &loc);

    /* Get the date */
    time(&t);
    localtime_r(&t, &date);

    fprintf(stdout, "Computing Prayer Times on %s", asctime(&date));
    fprintf(stdout, "Current location is: %s "
            "(Latitude = %f, Longitude = %f)\n",
            loc.name, loc.latitude, loc.longitude);

    qibla = get_qibla_direction(&loc);
    fprintf(stdout, "Qibla direction is %f degrees"
                    " from North clockwise\n", qibla);

    fprintf(stdout, "Current Timezone is UTC%s%.2f"
                    " (daylight is %s)\n",
            (loc.timezone >= 0? "+" : "-"),
            loc.timezone, (loc.daylight == 1? "on" : "off"));
    fprintf(stdout, "Calculation Method Used: %s, Fajr angle: %.2f,"
            " Isha %s: %.2f\n",
            loc.calc_method.name, loc.calc_method.fajr,
            (loc.calc_method.isha_type == ANGLE ?\
             "angle" : "offset"),
            loc.calc_method.isha);

    get_prayer_times(&date, &loc, &pt);

    fprintf(stdout, "\n Fajr\t\tSunrise\t\tDhuhr"
                    "\t\tAsr\t\tSunset\t\tIsha\n");
    fprintf(stdout, "--------------------------"
                    "--------------------------");
    fprintf(stdout, "---------------------------------\n");
    fprintf(stdout, " %d:%02d\t\t%d:%02d\t\t%d:%02d\t\t"
            "%d:%02d\t\t%d:%02d\t\t%d:%02d\n\n",
            pt.fajr.hour, pt.fajr.minute,
            pt.sunrise.hour, pt.sunrise.minute,
            pt.dhuhr.hour, pt.dhuhr.minute,
            pt.asr.hour, pt.asr.minute,
            pt.maghrib.hour, pt.maghrib.minute,
            pt.isha.hour, pt.isha.minute);


    return EXIT_SUCCESS;
}
