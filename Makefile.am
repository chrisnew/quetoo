ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	deps \
	src

RSYNC_REPOSITORY = \
	rsync://quetoo.org/quetoo

rsync-data:
	mkdir -p $(DESTDIR)@PKGDATADIR@
	rsync -rzhP --delete $(RSYNC_REPOSITORY) $(DESTDIR)@PKGDATADIR@
