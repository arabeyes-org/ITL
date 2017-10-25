# ITL
[![Build Status](https://travis-ci.org/arabeyes-org/ITL.svg?branch=master)](https://travis-ci.org/arabeyes-org/ITL)

The Islamic Tools and Libraries (ITL) project provides a fully featured library for performing common Islamic calculations.

ITL is currently composed of two library-modules:

  + **prayertime**: Geographically-aware prayer time and Qibla calculation routines.
  + **hijri**: Hijri date generation/calculation and conversion routines.

## Installation

Perform the following commands:
  * git clone https://github.com/arabeyes-org/ITL.git
  * mkdir build
  * cd build
  * cmake ..
  * make
  * make install (optional)

Alternatively, one can build using the autoconf build system as follows:
  * autoreconf -f -i
  * ./configure
  * make
  * make check (optional to run tests)
  * make install (optional)

## Other Languages

| Language 	| Type                 	| URL                                         	|
|----------	|----------------------	|---------------------------------------------	|
| PHP      	| Official port        	| https://github.com/arabeyes-org/ITL-ports   	|
| Obj-C    	| Unofficial interface 	| https://github.com/batoulapps/BAPrayerTimes 	|
| Java     	| Unofficial port      	| https://github.com/fikr4n/itl-java          	|
| GObject  	| Unofficial interface 	| https://github.com/aelmahmoudy/libitl-gobject |
