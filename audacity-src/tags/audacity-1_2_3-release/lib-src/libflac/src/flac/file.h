/* flac - Command-line FLAC encoder/decoder
 * Copyright (C) 2001,2002  Josh Coalson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef flac__file_h
#define flac__file_h

#include <sys/types.h> /* for off_t */
#include <stdio.h> /* for FILE */

void flac__file_copy_metadata(const char *srcpath, const char *destpath);
off_t flac__file_get_filesize(const char *srcpath);
const char *flac__file_get_basename(const char *srcpath);

/* these will forcibly set stdin/stdout to binary mode (for OSes that require it) */
FILE *file__get_binary_stdin();
FILE *file__get_binary_stdout();

#endif
