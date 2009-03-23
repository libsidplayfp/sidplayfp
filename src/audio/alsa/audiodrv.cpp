// --------------------------------------------------------------------------
// Advanced Linux Sound Architecture (ALSA) specific audio driver interface.
// --------------------------------------------------------------------------
/***************************************************************************
 *  $Log: audiodrv.cpp,v $
 *  Revision 1.10  2006/10/16 17:44:24  s_a_white
 *  Recover from a ctrl-z, bg which causes a buffer underrun during playback
 *
 *  Revision 1.9  2005/10/17 08:30:55  s_a_white
 *  Gentoo bug #98769.  goto open_error crosses initialization of
 *  `unsigned int rate'
 *
 *  Revision 1.8  2005/07/18 19:46:43  s_a_white
 *  Switch from obsolete alsa interface (patch by shd).
 *
 *  Revision 1.7  2002/03/04 19:07:48  s_a_white
 *  Fix C++ use of nothrow.
 *
 *  Revision 1.6  2002/01/10 19:04:00  s_a_white
 *  Interface changes for audio drivers.
 *
 *  Revision 1.5  2001/12/11 19:38:13  s_a_white
 *  More GCC3 Fixes.
 *
 *  Revision 1.4  2001/01/29 01:17:30  jpaana
 *  Use int_least8_t instead of ubyte_sidt which is obsolete now
 *
 *  Revision 1.3  2001/01/23 21:23:23  s_a_white
 *  Replaced SID_HAVE_EXCEPTIONS with HAVE_EXCEPTIONS in new
 *  drivers.
 *
 *  Revision 1.2  2001/01/18 18:35:41  s_a_white
 *  Support for multiple drivers added.  C standard update applied (There
 *  should be no spaces before #)
 *
 *  Revision 1.1  2001/01/08 16:41:43  s_a_white
 *  App and Library Seperation
 *
 ***************************************************************************/

#include "audiodrv.h"
#ifdef   HAVE_ALSA

#include <errno.h>
#include <stdio.h>
#ifdef HAVE_EXCEPTIONS
#   include <new>
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

void *Audio_ALSA::open (AudioConfig &cfg, const char *)
{
    AudioConfig tmpCfg;

    if (_audioHandle != NULL)
    {
        _errorString = "ERROR: Device already in use";
        return NULL;
     }

#ifdef HAVE_ALSA_ASOUNDLIB_H
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

    snd_pcm_format_t alsamode;
    switch (tmpCfg.precision)
    {
    case 8:
        tmpCfg.encoding = AUDIO_UNSIGNED_PCM;
        alsamode = SND_PCM_FORMAT_U8;
        break;
    case 16:
        tmpCfg.encoding = AUDIO_SIGNED_PCM;
        alsamode = SND_PCM_FORMAT_S16;
        break;
    default:
        _errorString = "ERROR: set desired number of bits for audio device.";
        goto open_error;
    }

    if (snd_pcm_hw_params_set_format (_audioHandle, hw_params, alsamode))
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

    _alsa_to_frames_divisor = tmpCfg.channels * tmpCfg.precision / 8;
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
#else // HAVE_ALSA_ASOUNDLIB_H
    // Obsolete interface
    int mask, wantedFormat, format;
    int rtn;
    int card = -1, dev = 0;
   
    if ((rtn = snd_pcm_open_preferred (&_audioHandle, &card, &dev, SND_PCM_OPEN_PLAYBACK)))
    {
        _errorString = "ERROR: Could not open audio device.";
        goto open_error;
    }
    
    // Transfer input parameters to this object.
    // May later be replaced with driver defaults.
    tmpCfg = cfg;

    snd_pcm_channel_params_t pp;
    snd_pcm_channel_setup_t setup;
 
    snd_pcm_channel_info_t pi;
   
    memset (&pi, 0, sizeof (pi));
    pi.channel = SND_PCM_CHANNEL_PLAYBACK;
    if ((rtn = snd_pcm_plugin_info (_audioHandle, &pi)))
    {
        _errorString = "ALSA: snd_pcm_plugin_info failed.";
        goto open_error;
    }
			
    memset(&pp, 0, sizeof (pp));
	
    pp.mode = SND_PCM_MODE_BLOCK;
    pp.channel = SND_PCM_CHANNEL_PLAYBACK;
    pp.start_mode = SND_PCM_START_FULL;
    pp.stop_mode = SND_PCM_STOP_STOP;
				     
    pp.buf.block.frag_size = pi.max_fragment_size;

    pp.buf.block.frags_max = 1;
    pp.buf.block.frags_min = 1;
   
    pp.format.interleave = 1;
    pp.format.rate = tmpCfg.frequency;
    pp.format.voices = tmpCfg.channels;
   
    // Set sample precision and type of encoding.
    if ( tmpCfg.precision == 8 )
    {
        tmpCfg.encoding = AUDIO_UNSIGNED_PCM;
        pp.format.format = SND_PCM_SFMT_U8;
    }
    if ( tmpCfg.precision == 16 )
    {
        tmpCfg.encoding = AUDIO_SIGNED_PCM;
        pp.format.format = SND_PCM_SFMT_S16_LE;
    }

    if ((rtn = snd_pcm_plugin_params (_audioHandle, &pp)) < 0)
    {
        _errorString = "ALSA: snd_pcm_plugin_params failed.";
        goto open_error;
    }
   
    if ((rtn = snd_pcm_plugin_prepare (_audioHandle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
    {
        _errorString = "ALSA: snd_pcm_plugin_prepare failed.";
        goto open_error;
    }
   
    memset (&setup, 0, sizeof (setup));
    setup.channel = SND_PCM_CHANNEL_PLAYBACK;
    if ((rtn = snd_pcm_plugin_setup (_audioHandle, &setup)) < 0)
    {
        _errorString = "ALSA: snd_pcm_plugin_setup failed.";
        goto open_error;
    }

    tmpCfg.bufSize = setup.buf.block.frag_size;
#endif
    
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) int_least8_t[tmpCfg.bufSize];
#else
    _sampleBuffer = new int_least8_t[tmpCfg.bufSize];
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
    return _sampleBuffer;

open_error:
#ifdef HAVE_ALSA_ASOUNDLIB_H
    if (hw_params)
        snd_pcm_hw_params_free (hw_params);
#endif
    if (_audioHandle != NULL)
        close ();
    perror ("ALSA");
    return NULL;
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void Audio_ALSA::close ()
{
    if (_audioHandle != NULL )
    {
        snd_pcm_close(_audioHandle);
        delete [] ((int_least8_t *) _sampleBuffer);
        outOfOrder ();
    }
}

void *Audio_ALSA::reset ()
{
    return (void *) _sampleBuffer;   
}

void *Audio_ALSA::write ()
{
    if (_audioHandle == NULL)
    {
        _errorString = "ERROR: Device not open.";
        return NULL;
    }

#ifdef HAVE_ALSA_ASOUNDLIB_H
    if (snd_pcm_writei  (_audioHandle, _sampleBuffer, _settings.bufSize / _alsa_to_frames_divisor) == -EPIPE)
        snd_pcm_prepare (_audioHandle); // Underrun
#else // Obsolete interface
    snd_pcm_plugin_write (_audioHandle, _sampleBuffer, _settings.bufSize);
#endif
    return (void *) _sampleBuffer;
}

#endif // HAVE_ALSA
