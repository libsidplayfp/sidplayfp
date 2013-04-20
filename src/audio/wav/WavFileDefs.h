/*
 * This file is part of sidplayfp, a SID player.
 *
 * Copyright 2011-2013 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2000-2001 Simon White
 * Copyright 2000 Michael Schwendt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef WAV_FILE_DEFS_H
#define WAV_FILE_DEFS_H

/*
 * A basic WAV output file type - Preprocessor definitions.
 * Initial implementation by Michael Schwendt <mschwendt@yahoo.com>
 */

#include "config.h"

#undef WAV_WORDS_BIGENDIAN
#if defined(WORDS_BIGENDIAN)
  #define WAV_WORDS_BIGENDIAN
#endif

#undef HAVE_IOS_BIN
#if defined(HAVE_IOS_BIN)
  #define WAV_HAVE_IOS_BIN
#endif

#undef WAV_HAVE_EXCEPTIONS
#ifdef HAVE_EXCEPTIONS
  #define WAV_HAVE_EXCEPTIONS
#endif

#undef WAV_HAVE_BAD_COMPILER
#ifdef HAVE_BAD_COMPILER
  #define HAVE_BAD_COMPILER
#endif

#undef WAV_HAVE_IOS_OPENMODE
#ifdef HAVE_IOS_OPENMODE
  #define WAV_HAVE_IOS_OPENMODE
#endif

/* Whether to revert any changes applied to the endian-ess of the
   non-const sample buffer contents after they have been written. */
#undef WAV_REVERT_BUFFER_CHANGES

#endif /* WAV_FILE_DEFS_H */
