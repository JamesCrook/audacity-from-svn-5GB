#  test_libOggFLAC++ - Unit tester for libOggFLAC++
#  Copyright (C) 2002  Josh Coalson
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

EXTRA_DIST = \
	Makefile.lite \
	Makefile.vc \
	README

CFLAGS = @CFLAGS@ @OGG_CFLAGS@

noinst_PROGRAMS = test_libOggFLAC++
test_libOggFLAC___LDADD = $(top_builddir)/src/libOggFLAC++/libOggFLAC++.la $(top_builddir)/src/libOggFLAC/libOggFLAC.la $(top_builddir)/src/libFLAC/libFLAC.la @OGG_LIBS@ -lm
test_libOggFLAC___SOURCES = \
	decoders.cc \
	encoders.cc \
	file_utils.c \
	main.cc \
	decoders.h \
	encoders.h \
	file_utils.h
