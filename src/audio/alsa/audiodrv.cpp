/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2013 Leandro Nini
 * Copyright 2000-2006 Simon White
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

#ifdef HAVE_ALSA

#include <stdio.h>

#ifdef HAVE_EXCEPTIONS
#  include <new>
#endif

Audio_ALSA::Audio_ALSA()
{
    // Reset everything.
    outOfOrder();
}

Audio_ALSA::~Audio_ALSA ()
{
    close ();
}

void Audio_ALSA::outOfOrder ()
{
    // Reset everything.
    _errorString = "None";
    _audioHandle = NULL;
}

bool Audio_ALSA::open (AudioConfig &cfg, const char *)
{
    snd_pcm_hw_params_t *hw_params = 0;

    try
    {
        if (_audioHandle != NULL)
        {
            throw error("ERROR: Device already in use");
        }

        if (snd_pcm_open (&_audioHandle, "default", SND_PCM_STREAM_PLAYBACK, 0))
        {
           throw error("ERROR: Could not open audio device.");
        }

        // May later be replaced with driver defaults.
        AudioConfig tmpCfg = cfg;

        if (snd_pcm_hw_params_malloc (&hw_params))
        {
            throw error("ERROR: could not malloc hwparams.");
        }

        if (snd_pcm_hw_params_any (_audioHandle, hw_params))
        {
            throw error("ERROR: could not initialize hw params");
        }

        if (snd_pcm_hw_params_set_access (_audioHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED))
        {
            throw error("ERROR: could not set access type");
        }

        if (snd_pcm_hw_params_set_format (_audioHandle, hw_params, SND_PCM_FORMAT_S16_LE))
        {
            throw error("ERROR: could not set sample format");
        }

        if (snd_pcm_hw_params_set_channels (_audioHandle, hw_params, tmpCfg.channels))
        {
            throw error("ERROR: could not set channel count");
        }

        {   // Gentoo bug #98769, comment 4
            unsigned int rate = tmpCfg.frequency;
            if (snd_pcm_hw_params_set_rate_near (_audioHandle, hw_params, &rate, 0))
            {
                throw error("ERROR: could not set sample rate");
            }
        }

        _alsa_to_frames_divisor = tmpCfg.channels;
        snd_pcm_uframes_t buffer_frames = 4096;
        snd_pcm_hw_params_set_period_size_near(_audioHandle, hw_params, &buffer_frames, 0);

        if (snd_pcm_hw_params (_audioHandle, hw_params))
        {
            throw error("ERROR: could not set hw parameters");
        }

        snd_pcm_hw_params_free (hw_params);
        hw_params = 0;

        if (snd_pcm_prepare (_audioHandle))
        {
            throw error("ERROR: could not prepare audio interface for use");
        }

        tmpCfg.bufSize = buffer_frames * _alsa_to_frames_divisor;
#ifdef HAVE_EXCEPTIONS
        _sampleBuffer = new(std::nothrow) short[tmpCfg.bufSize];
#else
        _sampleBuffer = new short[tmpCfg.bufSize];
#endif

        if (!_sampleBuffer)
        {
            throw error("AUDIO: Unable to allocate memory for sample buffers.");
        }

        // Setup internal Config
        _settings = tmpCfg;
        // Update the users settings
        getConfig (cfg);
        return true;
    }
    catch(error &e)
    {
        _errorString = e.message();

        if (hw_params)
            snd_pcm_hw_params_free (hw_params);
        if (_audioHandle != NULL)
            close ();
        perror ("ALSA");
        return false;
    }
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void Audio_ALSA::close ()
{
    if (_audioHandle != NULL )
    {
        snd_pcm_close(_audioHandle);
        delete[] _sampleBuffer;
        outOfOrder ();
    }
}

bool Audio_ALSA::write ()
{
    if (_audioHandle == NULL)
    {
        _errorString = "ERROR: Device not open.";
        return false;
    }

    if (snd_pcm_writei  (_audioHandle, _sampleBuffer, _settings.bufSize / _alsa_to_frames_divisor) == -EPIPE)
        snd_pcm_prepare (_audioHandle); // Underrun
    return true;
}

#endif // HAVE_ALSA
