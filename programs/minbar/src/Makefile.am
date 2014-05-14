bin_PROGRAMS = minbar

minbar_SOURCES = main.c main.h defines.h prefs.h locations-xml.c locations-xml.h config.c config.h

minbar_CPPFLAGS = 				\
	-DG_LOG_DOMAIN=\"Minbar\"		\
	-DGNOMELOCALEDIR=\""$(data)/locale"\"	\
	-DMINBAR_PIXMAPSDIR=\"$(data)/pixmaps\" \
	-DMINBAR_DATADIR=\"$(data)/minbar\"		\
	$(DISABLE_DEPRECATED)			\
	$(AM_CPPFLAGS)

minbar_CFLAGS = \
	   $(MINBAR_CFLAGS)	\
	   $(WARN_CFLAGS)	\
	   $(AM_CFLAGS) 	\
	   $(RSVG_CFLAGS)

minbar_LDFLAGS = \
	$(AM_LDFLAGS) -export-dynamic

minbar_LDADD = \
	$(MINBAR_LIBS)		\
	$(INTLLIBS)		\
	$(RSVG_LIBS)

if USE_NOTIFY
minbar_LDADD += $(NOTIFY_LIBS)
minbar_CPPFLAGS += $(NOTIFY_CFLAGS)
endif

if USE_GSTREAMER
minbar_LDADD += $(GSTREAMER_LIBS)
minbar_CPPFLAGS += $(GSTREAMER_CFLAGS)
else
minbar_LDADD += $(XINE_LIBS)
minbar_CPPFLAGS += $(XINE_CFLAGS)
endif

if PANEL_APPLET
bin_PROGRAMS += minbar-applet

minbar_applet_SOURCES = applet.c prefs.h
minbar_applet_CPPFLAGS = $(AM_CPPFLAGS) $(GCONF_CFLAGS) -DGNOMELOCALEDIR=\""$(data)/locale"\" -DDATADIR=\""$(datadir)/minbar-applet/"\"
minbar_applet_CFLAGS = $(MINBAR_CFLAGS) $(WARN_CFLAGS) $(AM_CFLAGS) $(APPLET_CFLAGS)
minbar_applet_LDFLAGS = $(AM_LDFLAGS) 
minbar_applet_LDADD = $(MINBAR_LIBS) $(GCONF_LIBS) $(APPLET_LIBS)
endif

DISTCLEANFILES = Makefile.in
