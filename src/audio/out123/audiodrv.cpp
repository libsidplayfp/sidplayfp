/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2016 Leandro Nini
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

#include <new>

Audio_OUT123::Audio_OUT123() :
    AudioBase("OUT123")
{
    // Reset everything.
    outOfOrder();
}

Audio_OUT123::~Audio_OUT123()
{
    close();
}

void Audio_OUT123::outOfOrder()
{
    // Reset everything.
    clearError();
    _audiofd = nullptr;
}

bool Audio_OUT123::open(AudioConfig &cfg)
{
    if (_audiofd != nullptr)
    {
        setError("Device already in use");
        return false;
    }

    try
    {
        if ((_audiofd = out123_new()) == NULL)
        {
            throw error("Could not init audio driver.");
        }

        if (out123_open(_audiofd, NULL, NULL) == (-1))
        {
            throw error(out123_strerror(_audiofd));
        }

        // Verify and accept the number of channels the driver accepted.
        switch (cfg.channels)
        {
        case 1:
        case 2:
        break;
        default:
            throw error("Could not set mono/stereo.");
        break;
        }

        if (out123_start(_audiofd, cfg.frequency, cfg.channels, MPG123_ENC_SIGNED_16) < 0)
        {
            throw error(out123_strerror(_audiofd));
        }

        cfg.bufSize = 8192; // FIXME make configurable?

        try
        {
            _sampleBuffer = new short[cfg.bufSize];
        }
        catch (std::bad_alloc const &ba)
        {
            throw error("Unable to allocate memory for sample buffers.");
        }

        // Setup internal Config
        _settings = cfg;
        return true;
    }
    catch(error const &e)
    {
        setError(e.message());

        if (_audiofd != nullptr)
        {
            close ();
            _audiofd = nullptr;
        }

        return false;
    }
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void Audio_OUT123::close()
{
    if (_audiofd != nullptr)
    {
        out123_del(_audiofd);
        delete [] _sampleBuffer;
        outOfOrder();
    }
}

void Audio_OUT123::reset()
{
    if (_audiofd != nullptr)
    {
        out123_stop(_audiofd);
    }
}

bool Audio_OUT123::write()
{
    if (_audiofd == nullptr)
    {
        setError("Device not open.");
        return false;
    }

    out123_play(_audiofd, _sampleBuffer, 2 * _settings.bufSize);
    // FIXME check return value?
    return true;
}
