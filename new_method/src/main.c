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
    double qibla;
    struct prayer_times pt;
    output_t output;

    output = parse_arguments(argc, argv, &loc, &date);

    qibla = get_qibla_direction(&loc);
    get_prayer_times(&date, &loc, &pt);

    if (output == OUTPUT_NORMAL) {
        fprintf(stdout, "Computing Prayer Times on %s",
                asctime(&date));
        fprintf(stdout, "Current location is: %s "
                "(Latitude = %f, Longitude = %f)\n",
                loc.name, loc.latitude, loc.longitude);

        fprintf(stdout, "Qibla direction is %f degrees"
                        " from North clockwise\n", qibla);

        fprintf(stdout, "Current Timezone is UTC%s%.2f"
                        " (daylight is %s)\n",
                (loc.timezone >= 0? "+" : "-"),
                loc.timezone, (loc.daylight == 1? "on" : "off"));
        fprintf(stdout, "Calculation Method Used: "
                        "%s, Fajr angle: %.2f,"
                        " Isha %s: %.2f\n",
                    loc.calc_method.name, loc.calc_method.fajr,
                    (loc.calc_method.isha_type == ANGLE ?\
                     "angle" : "offset"),
                    loc.calc_method.isha);

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
    } else if (output == OUTPUT_JSON) {
        fprintf(stdout, "{\n");
        fprintf(stdout, "\t\"fajr\": \"%d:%02d\",\n"
                        "\t\"dhuhr\": \"%d:%02d\",\n"
                        "\t\"sunrise\": \"%d:%02d\",\n"
                        "\t\"asr\": \"%d:%02d\",\n"
                        "\t\"maghrib\": \"%d:%02d\",\n"
                        "\t\"isha\": \"%d:%02d\"\n"
                        "}\n",
                pt.fajr.hour, pt.fajr.minute,
                pt.sunrise.hour, pt.sunrise.minute,
                pt.dhuhr.hour, pt.dhuhr.minute,
                pt.asr.hour, pt.asr.minute,
                pt.maghrib.hour, pt.maghrib.minute,
                pt.isha.hour, pt.isha.minute);
    }

    return EXIT_SUCCESS;
}
