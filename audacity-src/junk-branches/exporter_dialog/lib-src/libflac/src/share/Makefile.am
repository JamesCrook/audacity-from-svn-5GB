## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(top_srcdir)/include/share

noinst_LIBRARIES = libutf8.a libgetopt.a

libutf8_a_SOURCES = charset.c charset.h iconvert.c utf8.c

libgetopt_a_SOURCES = getopt.c getopt1.c

EXTRA_DIST = \
	Makefile.lite \
	Makefile.vc \
	README \
	charmaps.h \
	makemap.c \
	charset_test.c \
	charsetmap.h

debug:
	$(MAKE) all CFLAGS="@DEBUG@"

profile:
	$(MAKE) all CFLAGS="@PROFILE@"
