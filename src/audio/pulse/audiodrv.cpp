/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2008 Antti Lankila
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

#include "audiodrv.h"

#ifdef HAVE_PULSE

#ifdef HAVE_EXCEPTIONS
#  include <new>
#endif

Audio_Pulse::Audio_Pulse()
{
    outOfOrder();
}

Audio_Pulse::~Audio_Pulse ()
{
    close ();
}

void Audio_Pulse::outOfOrder ()
{
    _sampleBuffer = NULL;
    _errorString = "None";
}

bool Audio_Pulse::open (AudioConfig &cfg, const char *)
{
    pa_sample_spec pacfg = {};

    pacfg.channels = cfg.channels;
    pacfg.rate = cfg.frequency;
    pacfg.format = PA_SAMPLE_S16NE;

    // Set sample precision and type of encoding.
    _audioHandle = pa_simple_new(
        NULL,
        "sidplay",
        PA_STREAM_PLAYBACK,
        NULL,
        "Music",
        &pacfg,
        NULL,
        NULL,
        NULL
    );

    if (! _audioHandle) {
        _errorString = "Error acquiring pulseaudio stream";
        goto open_error;
    }

    cfg.bufSize = 4096;

#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) short[cfg.bufSize];
#else
    _sampleBuffer = new short[cfg.bufSize];
#endif

    if (!_sampleBuffer) {
        _errorString = "AUDIO: Unable to allocate memory for sample buffers.";
        goto open_error;
    }

    _settings = cfg;

    return true;

open_error:
    if (_audioHandle)
        pa_simple_free(_audioHandle);
    _audioHandle = NULL;

    return false;
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void Audio_Pulse::close ()
{
    if (_audioHandle != NULL) {
        pa_simple_free(_audioHandle);
        _audioHandle = NULL;
    }

    if (_sampleBuffer != NULL) {
        delete [] _sampleBuffer;
        outOfOrder ();
    }
}

bool Audio_Pulse::write ()
{
    if (_audioHandle == NULL)
    {
        _errorString = "ERROR: Device not open.";
        return false;
    }

    if (pa_simple_write(_audioHandle, _sampleBuffer, _settings.bufSize * 2, NULL) < 0) {
        _errorString = "Error writing to PA.";
    }
    return true;
}

#endif // HAVE_OSS
