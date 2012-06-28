
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS = @CWFLAGS@
LDADD = @LIBINTL@

sbin_PROGRAMS = \
	msgpack-autosplit

msgpack_autosplit_SOURCES = \
	app.c \
	gettext.h \
	log.c \
	log.h \
	msgpack-autosplit.h \
	options.c \
	options.h \
	safe_rw.c \
	safe_rw.h
