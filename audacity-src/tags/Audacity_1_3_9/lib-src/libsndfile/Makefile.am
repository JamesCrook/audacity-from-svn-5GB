## Process this file with automake to produce Makefile.in

DISTCHECK_CONFIGURE_FLAGS = --enable-gcc-werror

if BUILD_OCTAVE_MOD
octave_dir = Octave
endif

SUBDIRS = M4 man doc Win32 src $(octave_dir) examples regtest tests programs
DIST_SUBDIRS = M4 man doc Win32 src Octave examples regtest tests programs

EXTRA_DIST = libsndfile.spec.in sndfile.pc.in Mingw-make-dist.sh


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sndfile.pc

m4datadir = $(datadir)/aclocal

test: check-recursive

# Target to make autogenerated files.
genfiles :
	(cd src ; make genfiles)
	(cd tests ; make genfiles)


