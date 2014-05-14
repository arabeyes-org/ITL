SUBDIRS = src pixmaps data po doc

desktopdir = $(datadir)/applications
desktop_in_files = minbar.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

#distuninstallcheck_listfiles = find . -type f -print | grep -v scrollkeeper

EXTRA_DIST = \
	autogen.sh		\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	HACKING			\
	AUTHORS			\
	NEWS			\
	$(desktop_in_files)	

CLEANFILES = \
	$(desktop_DATA)

DISTCLEANFILES = \
	intltool-extract	\
	intltool-merge		\
	intltool-update	        \
	Makefile.in \
	xmldocs.make \
	depcomp \
	compile \
	mkinstalldirs \
	intltool-extract.in \
	intltool-update.in \
	omf.make \
	intltool-merge.in \
	missing \
	aclocal.m4 \
	install-sh \
	configure \
        po/Makefile.in.in

@INTLTOOL_DESKTOP_RULE@


