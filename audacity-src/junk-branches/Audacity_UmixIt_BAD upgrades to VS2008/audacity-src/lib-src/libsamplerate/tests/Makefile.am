noinst_PROGRAMS = misc_test termination_test simple_test reset_test \
					multi_channel_test snr_bw_test src-evaluate
	
SAMPLRATEDIR =../src
INCLUDES = -I$(srcdir)/$(SAMPLRATEDIR)
noinst_HEADERS = calc_snr.h util.h

#===============================================================================

misc_test_SOURCES = misc_test.c util.c util.h
misc_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(EFENCE_LIB)

termination_test_SOURCES = termination_test.c util.c util.h
termination_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(EFENCE_LIB)

simple_test_SOURCES = simple_test.c util.c util.h
simple_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(EFENCE_LIB)

reset_test_SOURCES = reset_test.c util.c util.h
reset_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(EFENCE_LIB)

multi_channel_test_SOURCES = multi_channel_test.c util.c util.h
multi_channel_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(EFENCE_LIB)

snr_bw_test_SOURCES = snr_bw_test.c calc_snr.c util.c util.h
snr_bw_test_LDADD = $(SAMPLRATEDIR)/libsamplerate.la $(FFTW_LIBS) $(EFENCE_LIB)

# This program is for evaluating other sample rate converters.

src_evaluate_SOURCES = src-evaluate.c calc_snr.c util.c
src_evaluate_CFLAGS = @SNDFILE_CFLAGS@
src_evaluate_LDADD = $(SNDFILE_LIBS) $(FFTW_LIBS)

#===============================================================================

check: termination_test snr_bw_test
	date
	uname -a
	./misc_test
	./termination_test
	./simple_test
	./reset_test
	./multi_channel_test
	./snr_bw_test
	@echo "-----------------------------------------------------------------"
	@echo "  ${PACKAGE}-${VERSION} passed all tests."
	@echo "-----------------------------------------------------------------"

