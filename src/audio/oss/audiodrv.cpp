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

#include "audiodrv.h"
#ifdef   HAVE_OSS

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_EXCEPTIONS
#  include <new>
#endif

#if defined(HAVE_NETBSD)
const char Audio_OSS::AUDIODEVICE[] = "/dev/audio";
#else
const char Audio_OSS::AUDIODEVICE[] = "/dev/dsp";
#endif

Audio_OSS::Audio_OSS()
{
    // Reset everything.
    outOfOrder();
    _swapEndian  = false;
}

Audio_OSS::~Audio_OSS ()
{
    close ();
}

void Audio_OSS::outOfOrder ()
{
    // Reset everything.
    _errorString = "None";
    _audiofd     = -1;
}

short *Audio_OSS::open (AudioConfig &cfg, const char *)
{
    int mask, format;
    int wantedFormat = 0;
    int temp = 0;

    if (_audiofd != -1)
    {
        _errorString = "ERROR: Device already in use";
        return NULL;
    }

    if (access (AUDIODEVICE, W_OK) == -1)
    {
        _errorString = "ERROR: Could locate an audio device.";
        goto open_error;
    }

    if ((_audiofd = ::open (AUDIODEVICE, O_WRONLY, 0)) == (-1))
    {
        _errorString = "ERROR: Could not open audio device.";
        goto open_error;
    }

    format = AFMT_S16_LE;
    if (ioctl (_audiofd, SNDCTL_DSP_SETFMT, &format) == (-1))
    {
        _errorString = "AUDIO: Could not set sample format.";
        goto open_error;
    }

    // Set mono/stereo.
    if (ioctl (_audiofd, SNDCTL_DSP_CHANNELS, &cfg.channels) == (-1))
    {
        _errorString = "AUDIO: Could not set mono/stereo.";
        goto open_error;
    }

    // Verify and accept the number of channels the driver accepted.
    switch (cfg.channels)
    {
    case 1:
    case 2:
    break;
    default:
        _errorString = "AUDIO: Could not set mono/stereo.";
        goto open_error;
    break;
    }

    // Set frequency.
    if (ioctl (_audiofd, SNDCTL_DSP_SPEED, &cfg.frequency) == (-1))
    {
        _errorString = "AUDIO: Could not set frequency.";
        goto open_error;
    }

    ioctl (_audiofd, SNDCTL_DSP_GETBLKSIZE, &temp);
    cfg.bufSize = (uint_least32_t) temp;
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) short[cfg.bufSize];
#else
    _sampleBuffer = new short[cfg.bufSize];
#endif

    if (!_sampleBuffer)
    {
        _errorString = "AUDIO: Unable to allocate memory for sample buffers.";
        goto open_error;
    }

    // Setup internal Config
    _settings = cfg;
return _sampleBuffer;

open_error:
    if (_audiofd != -1)
    {
        close ();
        _audiofd = -1;
    }

    perror (AUDIODEVICE);
return NULL;
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void Audio_OSS::close ()
{
    if (_audiofd != (-1))
    {
        ::close (_audiofd);
        delete [] _sampleBuffer;
        outOfOrder ();
    }
}

short *Audio_OSS::write ()
{
    //short tmp[_settings.bufSize];

    if (_audiofd == (-1))
    {
        _errorString = "ERROR: Device not open.";
        return NULL;
    }

   /* for (uint_least32_t n = 0; n < _settings.bufSize; n ++) {
            tmp[n] = _sampleBuffer[n] * (1 << 15);
    }*/

    ::write (_audiofd, _sampleBuffer, 2 * _settings.bufSize);
    return _sampleBuffer;
}

#endif // HAVE_OSS
