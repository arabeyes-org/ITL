# Process this file with automake to produce Makefile.in

NULL = 

CLEANFILES = $(NULL)

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libitl.la

pkginclude_HEADERS = \
	hijri/hijri.h \
	prayertime/prayer.h \
	$(NULL)

libitl_la_SOURCES = \
	hijri/hijri.c\
	hijri/hijri.h \
	hijri/umm_alqura.c \
	prayertime/astro.c \
	prayertime/astro.h \
	prayertime/prayer.c \
	prayertime/prayer.h \
	$(NULL)

libitl_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(ITL_LIBTOOL_VERSION_INFO) -no-undefined

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = itl.pc
CLEANFILES += $(pkgconfig_DATA)

noinst_PROGRAMS = demo_hijri demo_prayer

demo_hijri_SOURCES = hijri/demo_hijri.c
demo_hijri_LDADD = libitl.la

demo_prayer_SOURCES = prayertime/demo_prayer.c
demo_prayer_LDADD = libitl.la
