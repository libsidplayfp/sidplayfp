/*
 * This file is part of sidplayfp, a console SID player.
 *
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
    AudioConfig tmpCfg;

    if (_audioHandle != NULL)
    {
        _errorString = "ERROR: Device already in use";
        return false;
     }

    snd_pcm_uframes_t    buffer_frames;
    snd_pcm_hw_params_t *hw_params = 0;

    if (snd_pcm_open (&_audioHandle, "default", SND_PCM_STREAM_PLAYBACK, 0))
    {
        _errorString = "ERROR: Could not open audio device.";
        goto open_error;
    }

    // May later be replaced with driver defaults.
    tmpCfg = cfg;

    if (snd_pcm_hw_params_malloc (&hw_params))
    {
        _errorString = "ERROR: could not malloc hwparams.";
        goto open_error;
    }

    if (snd_pcm_hw_params_any (_audioHandle, hw_params))
    {
        _errorString = "ERROR: could not initialize hw params";
        goto open_error;
    }

    if (snd_pcm_hw_params_set_access (_audioHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED))
    {
        _errorString = "ERROR: could not set access type";
        goto open_error;
    }

    if (snd_pcm_hw_params_set_format (_audioHandle, hw_params, SND_PCM_FORMAT_S16_LE))
    {
        _errorString = "ERROR: could not set sample format";
        goto open_error;
    }

    if (snd_pcm_hw_params_set_channels (_audioHandle, hw_params, tmpCfg.channels))
    {
        _errorString = "ERROR: could not set channel count";
        goto open_error;
    }

    {   // Gentoo bug #98769, comment 4
        unsigned int rate = tmpCfg.frequency;
        if (snd_pcm_hw_params_set_rate_near (_audioHandle, hw_params, &rate, 0))
        {
            _errorString = "ERROR: could not set sample rate";
            goto open_error;
        }
    }

    _alsa_to_frames_divisor = tmpCfg.channels;
    buffer_frames = 4096;
    snd_pcm_hw_params_set_period_size_near(_audioHandle, hw_params, &buffer_frames, 0);

    if (snd_pcm_hw_params (_audioHandle, hw_params))
    {
        _errorString = "ERROR: could not set hw parameters";
        goto open_error;
    }

    snd_pcm_hw_params_free (hw_params);
    hw_params = 0;

    if (snd_pcm_prepare (_audioHandle))
    {
        _errorString = "ERROR: could not prepare audio interface for use";
        goto open_error;
    }

    tmpCfg.bufSize = buffer_frames * _alsa_to_frames_divisor;
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) short[tmpCfg.bufSize];
#else
    _sampleBuffer = new short[tmpCfg.bufSize];
#endif

    if (!_sampleBuffer)
    {
        _errorString = "AUDIO: Unable to allocate memory for sample buffers.";
        goto open_error;
    }

    // Setup internal Config
    _settings = tmpCfg;
    // Update the users settings
    getConfig (cfg);
    return true;

open_error:
    if (hw_params)
        snd_pcm_hw_params_free (hw_params);
    if (_audioHandle != NULL)
        close ();
    perror ("ALSA");
    return false;
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
