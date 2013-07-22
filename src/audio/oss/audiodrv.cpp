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

#include <new>

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

bool Audio_OSS::open (AudioConfig &cfg, const char *)
{
    if (_audiofd != -1)
    {
        _errorString = "ERROR: Device already in use";
        return false;
    }

    try
    {
        if (access (AUDIODEVICE, W_OK) == -1)
        {
            throw error("ERROR: Could locate an audio device.");
        }

        if ((_audiofd = ::open (AUDIODEVICE, O_WRONLY, 0)) == (-1))
        {
            throw error("ERROR: Could not open audio device.");
        }

        int format = AFMT_S16_LE;
        if (ioctl (_audiofd, SNDCTL_DSP_SETFMT, &format) == (-1))
        {
            throw error("AUDIO: Could not set sample format.");
        }

        // Set mono/stereo.
        if (ioctl (_audiofd, SNDCTL_DSP_CHANNELS, &cfg.channels) == (-1))
        {
            throw error("AUDIO: Could not set mono/stereo.");
        }

        // Verify and accept the number of channels the driver accepted.
        switch (cfg.channels)
        {
        case 1:
        case 2:
        break;
        default:
            throw error("AUDIO: Could not set mono/stereo.");
        break;
        }

        // Set frequency.
        if (ioctl (_audiofd, SNDCTL_DSP_SPEED, &cfg.frequency) == (-1))
        {
            throw error("AUDIO: Could not set frequency.");
        }

        int temp = 0;
        ioctl (_audiofd, SNDCTL_DSP_GETBLKSIZE, &temp);
        cfg.bufSize = (uint_least32_t) temp;

        try
        {
            _sampleBuffer = new short[cfg.bufSize];
        }
        catch (std::bad_alloc& ba)
        {
            throw error("AUDIO: Unable to allocate memory for sample buffers.");
        }

        // Setup internal Config
        _settings = cfg;
        return true;
    }
    catch(error &e)
    {
        _errorString = e.message();

        if (_audiofd != -1)
        {
            close ();
            _audiofd = -1;
        }

        perror (AUDIODEVICE);
        return false;
    }
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

bool Audio_OSS::write ()
{
    //short tmp[_settings.bufSize];

    if (_audiofd == (-1))
    {
        _errorString = "ERROR: Device not open.";
        return false;
    }

   /* for (uint_least32_t n = 0; n < _settings.bufSize; n ++) {
            tmp[n] = _sampleBuffer[n] * (1 << 15);
    }*/

    ::write (_audiofd, _sampleBuffer, 2 * _settings.bufSize);
    return true;
}

#endif // HAVE_OSS
