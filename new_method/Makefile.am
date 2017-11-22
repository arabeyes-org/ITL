
lib_LTLIBRARIES = libprayer.la
libprayer_la_CFLAGS = 
libprayer_la_LDFLAGS = -version-info $(LTVERSION)
libprayer_la_LIBADD= -lm
libprayer_la_SOURCES = src/prayer.c src/config.c
libprayer_la_HEADERS = src/prayer.h
libprayer_ladir = $(includedir)/prayer

check_PROGRAMS = prayer
prayer_LDADD= -lprayer -lm
prayer_SOURCES = src/main.c

TESTS=testwrapper.sh

exampledir = $(datarootdir)/doc/@PACKAGE@/new_method
example_DATA = $(prayer_SOURCES)
docdir = $(datadir)/doc/@PACKAGE@/new_method
doc_DATA = README.md
