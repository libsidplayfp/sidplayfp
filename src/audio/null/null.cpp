/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2000-2002 Simon White
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

#include "null.h"

#ifdef HAVE_EXCEPTIONS
#  include <new>
#endif

Audio_Null::Audio_Null ()
{
    isOpen = false;
}

Audio_Null::~Audio_Null ()
{
    close();
}

short *Audio_Null::open (AudioConfig &cfg, const char *)
{
    uint_least32_t bufSize = cfg.bufSize;

    if (isOpen)
    {
        _errorString = "NULL ERROR: Audio device already open.";
        return NULL;
    }

    if (bufSize == 0)
    {
        bufSize  = 4096;
    }

    // We need to make a buffer for the user
#if defined(HAVE_EXCEPTIONS)
    _sampleBuffer = new(std::nothrow) short[bufSize];
#else
    _sampleBuffer = new short[bufSize];
#endif
    if (!_sampleBuffer)
        return NULL;

    isOpen      = true;
    cfg.bufSize = bufSize;
    _settings   = cfg;
    return _sampleBuffer;
}

short *Audio_Null::write ()
{
    if (!isOpen)
    {
        _errorString = "NULL ERROR: Audio device not open.";
        return NULL;
    }
    return _sampleBuffer;
}

short *Audio_Null::reset (void)
{
    if (!isOpen)
         return NULL;
    return _sampleBuffer;
}

void Audio_Null::close (void)
{
    if (!isOpen)
        return;
    delete [] (uint_least8_t *) _sampleBuffer;
    _sampleBuffer = NULL;
    isOpen = false;
}
