/* Pulseaudio support -- written by Antti S. Lankila
 * (c) 2008, licensed under the GPL. See COPYING for details. */

#include "audiodrv.h"
#ifdef   HAVE_PULSE
#include <pulse/simple.h>

#include <stdio.h>
#ifdef HAVE_EXCEPTIONS
#   include <new>
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

void *Audio_Pulse::open (AudioConfig &cfg, const char *)
{
    pa_sample_spec pacfg = {};

    pacfg.channels = cfg.channels;
    pacfg.rate = cfg.frequency;
    if (cfg.precision == 16)
        pacfg.format = PA_SAMPLE_S16NE;
    else {
        _errorString = "16-bit only";
        return NULL;
    }

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
    _sampleBuffer = new(std::nothrow) int_least8_t[cfg.bufSize];
#else
    _sampleBuffer = new int_least8_t[cfg.bufSize];
#endif

    if (!_sampleBuffer) {
        _errorString = "AUDIO: Unable to allocate memory for sample buffers.";
        goto open_error;
    }

    _settings = cfg;

    return _sampleBuffer;

open_error:
    if (_audioHandle)
        pa_simple_free(_audioHandle);
    _audioHandle = NULL;

    return NULL;
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
        delete [] (int_least8_t *) _sampleBuffer;
        outOfOrder ();
    }
}

void *Audio_Pulse::write ()
{
    if (_audioHandle == NULL)
    {
        _errorString = "ERROR: Device not open.";
        return NULL;
    }

    if (pa_simple_write(_audioHandle, _sampleBuffer, _settings.bufSize, NULL) < 0) {
        _errorString = "Error writing to PA.";
    }
    return _sampleBuffer;
}

#endif // HAVE_OSS
