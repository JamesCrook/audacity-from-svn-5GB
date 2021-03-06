noinst_PROGRAMS = sfversion floating_point_test write_read_test \
	lossy_comp_test error_test ulaw_test alaw_test dwvw_test \
	peak_chunk_test command_test stdin_test stdout_test stdio_test \
	pcm_test headerless_test pipe_test benchmark misc_test raw_test \
	fix_this
	
SNDFILEDIR =../src
INCLUDES = -I$(srcdir)/$(SNDFILEDIR)
noinst_HEADERS = dft_cmp.h utils.h

autogen_sources = write_read_test.tpl write_read_test.def	\
					pcm_test.tpl pcm_test.def	\
					utils.tpl utils.def						\
					command_test.tpl command_test.def		\
					benchmark.tpl benchmark.def	

EXTRA_DIST = $(autogen_sources)

#===============================================================================

sfversion_SOURCES = sfversion.c 
sfversion_LDADD = $(SNDFILEDIR)/libsndfile.la

write_read_test_SOURCES = utils.c write_read_test.c
write_read_test_LDADD = $(SNDFILEDIR)/libsndfile.la

lossy_comp_test_SOURCES = utils.c lossy_comp_test.c
lossy_comp_test_LDADD = $(SNDFILEDIR)/libsndfile.la

fix_this_SOURCES = utils.c fix_this.c 
fix_this_LDADD = $(SNDFILEDIR)/libsndfile.la

error_test_SOURCES = error_test.c 
error_test_LDADD = $(SNDFILEDIR)/libsndfile.la

ulaw_test_SOURCES = utils.c ulaw_test.c
ulaw_test_LDADD = $(SNDFILEDIR)/libsndfile.la

alaw_test_SOURCES = utils.c alaw_test.c
alaw_test_LDADD = $(SNDFILEDIR)/libsndfile.la

dwvw_test_SOURCES = utils.c dwvw_test.c
dwvw_test_LDADD = $(SNDFILEDIR)/libsndfile.la

floating_point_test_SOURCES = utils.c dft_cmp.c floating_point_test.c
floating_point_test_LDADD = $(SNDFILEDIR)/libsndfile.la

peak_chunk_test_SOURCES = peak_chunk_test.c 
peak_chunk_test_LDADD = $(SNDFILEDIR)/libsndfile.la

command_test_SOURCES = command_test.c utils.c
command_test_LDADD = $(SNDFILEDIR)/libsndfile.la

pcm_test_SOURCES = utils.c pcm_test.c
pcm_test_LDADD = $(SNDFILEDIR)/libsndfile.la

headerless_test_SOURCES = utils.c headerless_test.c
headerless_test_LDADD = $(SNDFILEDIR)/libsndfile.la

stdin_test_SOURCES = utils.c stdin_test.c 
stdin_test_LDADD = $(SNDFILEDIR)/libsndfile.la

stdout_test_SOURCES = stdout_test.c 
stdout_test_LDADD = $(SNDFILEDIR)/libsndfile.la

stdio_test_SOURCES = stdio_test.c 
stdio_test_LDADD = 

pipe_test_SOURCES = pipe_test.c 
pipe_test_LDADD = 

benchmark_SOURCES = benchmark.c 
benchmark_LDADD = $(SNDFILEDIR)/libsndfile.la

misc_test_SOURCES = misc_test.c utils.c 
misc_test_LDADD = $(SNDFILEDIR)/libsndfile.la

raw_test_SOURCES = raw_test.c utils.c 
raw_test_LDADD = $(SNDFILEDIR)/libsndfile.la

#===============================================================================

write_read_test.c: write_read_test.def write_read_test.tpl
	autogen --writable --source-time write_read_test.def
	
pcm_test.c: pcm_test.def pcm_test.tpl
	autogen --writable --source-time pcm_test.def
	
utils.c: utils.def utils.tpl
	autogen --writable --source-time utils.def

command_test.c: command_test.def command_test.tpl
	autogen --writable --source-time command_test.def
	
benchmark.c: benchmark.def benchmark.tpl
	autogen --writable --source-time benchmark.def

#===============================================================================

check: generic-tests ulaw_test alaw_test floating_point_test \
		wav-tests aiff-tests au-tests raw-tests paf-tests svx-tests nist-tests \
		ircam-tests voc-tests w64-tests mat4-tests mat5-tests stdio-tests

generic-tests : error_test ulaw_test alaw_test command_test floating_point_test pcm_test
	uname -a
	./error_test
	./pcm_test
	./ulaw_test
	./alaw_test
	./dwvw_test
	./command_test ver
	./command_test norm
	./command_test format
	./command_test peak
	./command_test trunc
	./floating_point_test
	./headerless_test
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed common tests."
	@echo "----------------------------------------------------------------------"

wav-tests: write_read_test lossy_comp_test peak_chunk_test misc_test
	./write_read_test wav
	./lossy_comp_test wav_ima
	./lossy_comp_test wav_msadpcm
	./lossy_comp_test wav_ulaw
	./lossy_comp_test wav_alaw
	./lossy_comp_test wav_gsm610
	./peak_chunk_test wav
	./misc_test wav
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on WAV files."
	@echo "----------------------------------------------------------------------"

aiff-tests: write_read_test lossy_comp_test peak_chunk_test misc_test
	./write_read_test aiff
	./lossy_comp_test aiff_ulaw
	./lossy_comp_test aiff_alaw
	./lossy_comp_test aiff_gsm610
	./peak_chunk_test aiff
	./misc_test aiff
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on AIFF files."
	@echo "----------------------------------------------------------------------"

au-tests: write_read_test lossy_comp_test misc_test
	./write_read_test au
	./lossy_comp_test au_ulaw
	./lossy_comp_test au_alaw
	./lossy_comp_test au_g721
	./lossy_comp_test au_g723
	./misc_test au
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on AU files."
	@echo "----------------------------------------------------------------------"

raw-tests: write_read_test lossy_comp_test raw_test
	./write_read_test raw
	./lossy_comp_test raw_ulaw
	./lossy_comp_test raw_alaw
	./lossy_comp_test vox_adpcm
	./raw_test
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on RAW (header-less) files."
	@echo "----------------------------------------------------------------------"

paf-tests: write_read_test misc_test
	./write_read_test paf
	./misc_test paf
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on PAF files."
	@echo "----------------------------------------------------------------------"

svx-tests: write_read_test misc_test
	./write_read_test svx
	./misc_test svx
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on SVX files."
	@echo "----------------------------------------------------------------------"

nist-tests: write_read_test lossy_comp_test misc_test
	./write_read_test nist
	./lossy_comp_test nist_ulaw
	./lossy_comp_test nist_alaw
	./misc_test nist
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on NIST files."
	@echo "----------------------------------------------------------------------"

ircam-tests: write_read_test lossy_comp_test misc_test
	./write_read_test ircam
	./lossy_comp_test ircam_ulaw
	./lossy_comp_test ircam_alaw
	./misc_test ircam
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on IRCAM files."
	@echo "----------------------------------------------------------------------"

voc-tests: write_read_test lossy_comp_test misc_test
	./write_read_test voc
	./lossy_comp_test voc_ulaw
	./lossy_comp_test voc_alaw
	./misc_test voc
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on VOC files."
	@echo "----------------------------------------------------------------------"

w64-tests: write_read_test lossy_comp_test misc_test
	./write_read_test w64
	./lossy_comp_test w64_ima
	./lossy_comp_test w64_msadpcm
	./lossy_comp_test w64_ulaw
	./lossy_comp_test w64_alaw
	./lossy_comp_test w64_gsm610
	./misc_test w64
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on W64 files."
	@echo "----------------------------------------------------------------------"

mat4-tests: write_read_test misc_test
	./write_read_test mat4
	./misc_test mat4
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on MAT4 files."
	@echo "----------------------------------------------------------------------"

mat5-tests: write_read_test misc_test
	./write_read_test mat5
	./misc_test mat5
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed tests on MAT5 files."
	@echo "----------------------------------------------------------------------"

stdio-tests: stdio_test stdin_test stdout_test pipe_test
	./stdio_test
	./pipe_test
	@echo "----------------------------------------------------------------------"
	@echo "  `./sfversion` passed stdio and pipe tests."
	@echo "----------------------------------------------------------------------"


