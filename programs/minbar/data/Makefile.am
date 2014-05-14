dist_pkgdata_DATA = athan.ogg minbar.ui Locations.xml

if PANEL_APPLET
serverdir = $(libdir)/bonobo/servers
server_in_files = minbar-applet.server.in
server_DATA = $(server_in_files:.server.in=.server)

xmldir = $(datadir)/minbar-applet
xml_DATA = minbar-applet.xml
endif

if USE_RSVG
dist_pkgdata_DATA += Compass.svg
endif

org.gnome.minbar.gschema.xml.in: $(srcdir)/org.gnome.minbar.gschema.xml.in.in
	sed     -e 's,[@]datadir[@],$(datadir),g' \
		-e 's,[@]PACKAGE[@],$(PACKAGE),g' \
		<$(srcdir)/org.gnome.minbar.gschema.xml.in.in >org.gnome.minbar.gschema.xml.in

gsettingsschema_in_files = org.gnome.minbar.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)

@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

$(server_DATA): $(server_in_files)
	sed -e "s|\@BINDIR\@|$(bindir)|" $< > $@

EXTRA_DIST =						\
	$(gsettingsschema_in_files)			\
	$(NULL)

CLEANFILES = \
	$(gsettings_SCHEMAS) \
	$(server_DATA) \
	$(NULL)

DISTCLEANFILES = Makefile.in

## End Makefile.am
