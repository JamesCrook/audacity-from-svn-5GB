# This file was automatically generated
# DO NOT EDIT!

bin_PROGRAMS = sndfile-info sndfile-play sndfile-convert

noinst_PROGRAMS = make_sine sfprocess list_formats cooledit-fixer generate

# This is the BeOS version of sndfile-play. It needs to be compiled with the C++
# compiler.
EXTRA_DIST = sndfile-play-beos.cpp

OS_SPECIFIC_INCLUDES = @OS_SPECIFIC_INCLUDES@
OS_SPECIFIC_LINKS = @OS_SPECIFIC_LINKS@

SNDFILEDIR =../src
INCLUDES = -I$(srcdir)/$(SNDFILEDIR) $(OS_SPECIFIC_INCLUDES)

sndfile_info_SOURCES = sndfile-info.c 
sndfile_info_LDADD = $(SNDFILEDIR)/libsndfile.la

sndfile_play_SOURCES = sndfile-play.c 
sndfile_play_LDADD = $(SNDFILEDIR)/libsndfile.la $(OS_SPECIFIC_LINKS)

sndfile_convert_SOURCES = sndfile-convert.c 
sndfile_convert_LDADD = $(SNDFILEDIR)/libsndfile.la
 
make_sine_SOURCES = make_sine.c 
make_sine_LDADD = $(SNDFILEDIR)/libsndfile.la

sfprocess_SOURCES = sfprocess.c 
sfprocess_LDADD = $(SNDFILEDIR)/libsndfile.la

list_formats_SOURCES = list_formats.c 
list_formats_LDADD = $(SNDFILEDIR)/libsndfile.la

cooledit_fixer_SOURCES = cooledit-fixer.c 
cooledit_fixer_LDADD = $(SNDFILEDIR)/libsndfile.la

generate_SOURCES = generate.c 
generate_LDADD = $(SNDFILEDIR)/libsndfile.la
