## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_SOURCE_DIR=\"".."\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = namaztime

namaztime_SOURCES = \
	main.c \
	times.c times.h \
	support.c support.h \
	interface.c interface.h \
	eggtrayicon.c eggtrayicon.h \
	callbacks.c callbacks.h

namaztime_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -litl

