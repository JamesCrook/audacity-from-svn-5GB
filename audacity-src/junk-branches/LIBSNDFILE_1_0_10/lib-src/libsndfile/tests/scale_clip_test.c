/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <math.h>

#include <sndfile.h>

#include "utils.h"

#ifndef		M_PI
#define		M_PI	3.14159265358979323846264338
#endif

#define	HALF_BUFFER_SIZE	(1 << 12)
#define	BUFFER_SIZE			(2 * HALF_BUFFER_SIZE)

#define	SINE_AMP		1.1
#define	MAX_ERROR		0.0202


static void	flt_scale_clip_test_16 (const char *filename, int filetype, float maxval) ;
static void	flt_scale_clip_test_24 (const char *filename, int filetype, float maxval) ;
static void	flt_scale_clip_test_32 (const char *filename, int filetype, float maxval) ;
static void	flt_scale_clip_test_08 (const char *filename, int filetype, float maxval) ;

static void	dbl_scale_clip_test_16 (const char *filename, int filetype, float maxval) ;
static void	dbl_scale_clip_test_24 (const char *filename, int filetype, float maxval) ;
static void	dbl_scale_clip_test_32 (const char *filename, int filetype, float maxval) ;
static void	dbl_scale_clip_test_08 (const char *filename, int filetype, float maxval) ;


/* Data buffer. */
static	double	buffer_out	[BUFFER_SIZE] ;
static	double	buffer_in	[BUFFER_SIZE] ;

int
main (void)
{
	flt_scale_clip_test_08 ("scale_clip_s8.au", SF_FORMAT_AU | SF_FORMAT_PCM_S8, 1.0 * 0x80) ;
	flt_scale_clip_test_08 ("scale_clip_u8.wav", SF_FORMAT_WAV | SF_FORMAT_PCM_U8, 1.0 * 0x80) ;

	dbl_scale_clip_test_08 ("scale_clip_s8.au", SF_FORMAT_AU | SF_FORMAT_PCM_S8, 1.0 * 0x80) ;
	dbl_scale_clip_test_08 ("scale_clip_u8.wav", SF_FORMAT_WAV | SF_FORMAT_PCM_U8, 1.0 * 0x80) ;

	/*
	**	Now use SF_FORMAT_AU where possible because it allows both
	**	big and little endian files.
	*/

	flt_scale_clip_test_16 ("scale_clip_be16.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_16, 1.0 * 0x8000) ;
	flt_scale_clip_test_16 ("scale_clip_le16.au", SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_PCM_16, 1.0 * 0x8000) ;
	flt_scale_clip_test_24 ("scale_clip_be24.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_24, 1.0 * 0x800000) ;
	flt_scale_clip_test_24 ("scale_clip_le24.au", SF_ENDIAN_LITTLE	| SF_FORMAT_AU | SF_FORMAT_PCM_24, 1.0 * 0x800000) ;
	flt_scale_clip_test_32 ("scale_clip_be32.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_32, 1.0 * 0x80000000) ;
	flt_scale_clip_test_32 ("scale_clip_le32.au", SF_ENDIAN_LITTLE	| SF_FORMAT_AU | SF_FORMAT_PCM_32, 1.0 * 0x80000000) ;

	dbl_scale_clip_test_16 ("scale_clip_be16.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_16, 1.0 * 0x8000) ;
	dbl_scale_clip_test_16 ("scale_clip_le16.au", SF_ENDIAN_LITTLE	| SF_FORMAT_AU | SF_FORMAT_PCM_16, 1.0 * 0x8000) ;
	dbl_scale_clip_test_24 ("scale_clip_be24.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_24, 1.0 * 0x800000) ;
	dbl_scale_clip_test_24 ("scale_clip_le24.au", SF_ENDIAN_LITTLE	| SF_FORMAT_AU | SF_FORMAT_PCM_24, 1.0 * 0x800000) ;
	dbl_scale_clip_test_32 ("scale_clip_be32.au", SF_ENDIAN_BIG	| SF_FORMAT_AU | SF_FORMAT_PCM_32, 1.0 * 0x80000000) ;
	dbl_scale_clip_test_32 ("scale_clip_le32.au", SF_ENDIAN_LITTLE	| SF_FORMAT_AU | SF_FORMAT_PCM_32, 1.0 * 0x80000000) ;

	return 0 ;
} /* main */

/*============================================================================================
**	Here are the test functions.
*/


static void
flt_scale_clip_test_16 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	float		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("flt_scale_clip_test_16", filename) ;

	data_out = (float*) buffer_out ;
	data_in = (float*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_float_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_write_float_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_float_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_read_float_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x8000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* flt_scale_clip_test_16 */

static void
flt_scale_clip_test_24 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	float		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("flt_scale_clip_test_24", filename) ;

	data_out = (float*) buffer_out ;
	data_in = (float*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_float_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_write_float_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_float_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_read_float_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x800000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* flt_scale_clip_test_24 */

static void
flt_scale_clip_test_32 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	float		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("flt_scale_clip_test_32", filename) ;

	data_out = (float*) buffer_out ;
	data_in = (float*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_float_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_write_float_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_float_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_read_float_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x80000000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* flt_scale_clip_test_32 */

static void
flt_scale_clip_test_08 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	float		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("flt_scale_clip_test_08", filename) ;

	data_out = (float*) buffer_out ;
	data_in = (float*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_float_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_write_float_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_float_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_FLOAT, NULL, SF_FALSE) ;
	test_read_float_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x80)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* flt_scale_clip_test_08 */



static void
dbl_scale_clip_test_16 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	double		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("dbl_scale_clip_test_16", filename) ;

	data_out = (double*) buffer_out ;
	data_in = (double*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_double_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_write_double_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_double_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_read_double_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x8000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* dbl_scale_clip_test_16 */

static void
dbl_scale_clip_test_24 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	double		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("dbl_scale_clip_test_24", filename) ;

	data_out = (double*) buffer_out ;
	data_in = (double*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_double_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_write_double_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_double_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_read_double_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x800000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* dbl_scale_clip_test_24 */

static void
dbl_scale_clip_test_32 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	double		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("dbl_scale_clip_test_32", filename) ;

	data_out = (double*) buffer_out ;
	data_in = (double*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_double_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_write_double_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_double_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_read_double_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x80000000)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* dbl_scale_clip_test_32 */

static void
dbl_scale_clip_test_08 (const char *filename, int filetype, float maxval)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	double		*data_out, *data_in ;
	double		diff, clip_max_diff ;

	print_test_name ("dbl_scale_clip_test_08", filename) ;

	data_out = (double*) buffer_out ;
	data_in = (double*) buffer_in ;

	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	data_out [k] = SINE_AMP * sin (2 * M_PI * k / HALF_BUFFER_SIZE) ;
		data_out [k + HALF_BUFFER_SIZE] = data_out [k] * maxval ;
		} ;

	sfinfo.samplerate	= 44100 ;
	sfinfo.frames		= 123456789 ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels		= 1 ;
	sfinfo.format		= filetype ;

	/*
	**	Write two versions of the data:
	**		normalized and clipped
	**		un-normalized and clipped.
	*/

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_SET_CLIPPING, NULL, SF_TRUE) ;
	test_write_double_or_die (file, 0, data_out, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_write_double_or_die (file, 0, data_out + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	memset (buffer_in, 0, sizeof (buffer_in)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	sfinfo.format &= (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK) ;

	if (sfinfo.format != (filetype & (SF_FORMAT_TYPEMASK | SF_FORMAT_SUBMASK)))
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames != BUFFER_SIZE)
	{	printf ("\n\nLine %d: Incorrect number of frames in file (%d => %ld).\n\n", __LINE__, BUFFER_SIZE, SF_COUNT_TO_LONG (sfinfo.frames)) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file, __LINE__) ;

	test_read_double_or_die (file, 0, data_in, HALF_BUFFER_SIZE, __LINE__) ;
	sf_command (file, SFC_SET_NORM_DOUBLE, NULL, SF_FALSE) ;
	test_read_double_or_die (file, 0, data_in + HALF_BUFFER_SIZE, HALF_BUFFER_SIZE, __LINE__) ;
	sf_close (file) ;

	/* Check normalized version. */
	clip_max_diff = 0.0 ;
	for (k = 0 ; k < HALF_BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > 1.0)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > 1.0)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0 / 0x80)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	/* Check the un-normalized data. */
	clip_max_diff = 0.0 ;
	for (k = HALF_BUFFER_SIZE ; k < BUFFER_SIZE ; k++)
	{	if (fabs (data_in [k]) > maxval)
		{	printf ("\n\nLine %d: Input sample %d/%d (%f) has not been clipped.\n\n", __LINE__, k, BUFFER_SIZE, data_in [k]) ;
			exit (1) ;
			} ;

		if (data_out [k] * data_in [k] < 0.0)
		{	printf ("\n\nLine %d: Data wrap around at index %d/%d.\n\n", __LINE__, k, BUFFER_SIZE) ;
			exit (1) ;
			} ;

		if (fabs (data_out [k]) > maxval)
			continue ;

		diff = fabs (data_out [k] - data_in [k]) ;
		if (diff > clip_max_diff)
			clip_max_diff = diff ;
		} ;

	if (clip_max_diff < 1e-20)
	{	printf ("\n\nLine %d: Clipping difference (%e) too small (un-normalized).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	if (clip_max_diff > 1.0)
	{	printf ("\n\nLine %d: Clipping difference (%e) too large (un-normalised).\n\n", __LINE__, clip_max_diff) ;
		exit (1) ;
		} ;

	printf ("ok\n") ;
	unlink (filename) ;
} /* dbl_scale_clip_test_08 */





