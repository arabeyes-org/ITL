SUBDIRS=$(AM_SUBDIRS)
lib_LTLIBRARIES = libitl.la
libitl_la_CFLAGS = 
libitl_la_LDFLAGS = -version-info $(LTVERSION)
libitl_la_LIBADD= -lm
libitl_la_SOURCES = hijri/hijri.c prayertime/astro.c prayertime/prayer.c hijri/umm_alqura.c
libitl_la_HEADERS = hijri/hijri.h prayertime/prayer.h
libitl_ladir = $(includedir)/itl

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE@.pc

check_PROGRAMS = demo_hijri demo_prayer
demo_hijri_LDADD= -litl -lm
demo_hijri_SOURCES = hijri/demo_hijri.c
demo_prayer_LDADD= -litl -lm
demo_prayer_SOURCES = prayertime/demo_prayer.c

TESTS=demo_prayer demo_hijri

exampledir = $(datarootdir)/doc/@PACKAGE@
example_DATA = hijri/hijri.c prayertime/prayer.c
docdir = $(datadir)/doc/@PACKAGE@
doc_DATA = prayertime/README.md prayertime/doc/method-info.md
