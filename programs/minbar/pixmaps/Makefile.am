SUBDIRS = icons

#icondir = $(datadir)/icons/hicolor/48x48/apps
#icon_DATA = minbar.png

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

logodir = $(datadir)/minbar
logo_DATA = minbar-logo.png

EXTRA_DIST = \
        $(logo_DATA)


DISTCLEANFILES = Makefile.in
