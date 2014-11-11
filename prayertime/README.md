# Prayertime

The main goal of this library is to allow applications to generate
accurate prayer times based on the user's location and Fiqh
method settings.

To learn more about the library interface, read [prayer.h](prayer.h)

##  Supported Methods of calculation and Fiqh settings

By passing the appropriate values to the Method structure, the library
is able to support all known methods of prayer time calculations for
all the different schools of Fiqh there is. More importantly, the
library is not restricted to a certain Fiqh, but instead, the user is
fully able to customize the calculation method used based on their
own school of Fiqh, actual observations, or scientific research.

The library can auto-fill the values for the most commonly used 
methods in many countries around the world. The library also 
supports prayer time estimation for high latitudes, as
recommended by both traditional and modern scholarship.

See the [doc/method-info.md](doc/method-info.md) file for the complete details.

## Examples

The [demo_prayer.c](demo_prayer.c) file shows a small example of using the library
from the command line.

Included in the library is a "swig" directory that
shows an example of creating a wrapper library using SWIG and
accessing the library routines through a PERL script. (More info. at
www.swig.org)

## Limitations

The current True North Qibla formula is simplistic and there
is no support for Magnetic North Qibla calculations. 

**Needs improvement**

More documentation and better error checking for the prayer code.
