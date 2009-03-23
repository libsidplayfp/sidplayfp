// --------------------------------------------------------------------------
// SGI/Irix specific audio interface. (very poor)
// --------------------------------------------------------------------------
/***************************************************************************
 *  $Log: audiodrv.cpp,v $
 *  Revision 1.8  2004/05/24 20:30:01  s_a_white
 *  Integrate IRIX patch submitted by Marcus Herbert (Jan 2003) which was
 *  forgotten about and only recently stumbled upon.
 *
 *  Revision 1.7  2002/03/04 19:07:48  s_a_white
 *  Fix C++ use of nothrow.
 *
 *  Revision 1.6  2002/01/10 19:04:01  s_a_white
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
 *  Revision 1.2  2001/01/18 18:36:16  s_a_white
 *  Support for multiple drivers added.  C standard update applied (There
 *  should be no spaces before #)
 *
 *  Revision 1.1  2001/01/08 16:41:43  s_a_white
 *  App and Library Seperation
 *
 ***************************************************************************/

#include "audiodrv.h"
#ifdef   HAVE_IRIX

#ifdef HAVE_EXCEPTIONS
#   include <new>
#endif

#include <stdio.h>

#include "audiodrv.h"

Audio_Irix::Audio_Irix()
{
    outOfOrder();
}

Audio_Irix::~Audio_Irix()
{
    close();
}

void Audio_Irix::outOfOrder()
{
    // Reset everything.
    _errorString = "None";
    _audio       = NULL;
}

void *Audio_Irix::open (AudioConfig& cfg, const char *)
{
    // Copy input parameters. May later be replaced with driver defaults.
    _settings = cfg;
    
    _config = ALnewconfig();

    // Set sample format
    ALsetsampfmt(_config, AL_SAMPFMT_TWOSCOMP);

    // stereo or mono mode
    _settings.channels = cfg.channels >= 2 ? 2 : 1;
    if (_settings.channels == 2)
        ALsetchannels(_config, AL_STEREO);
    else
        ALsetchannels(_config, AL_MONO);

    // 16 or 8 bit sample
    _settings.precision = cfg.precision >= 16 ? 16 : 8;
    if (_settings.precision == 16)
        ALsetwidth(_config, AL_SAMPLE_16);
    else
        ALsetwidth(_config, AL_SAMPLE_8);

    // Frequency
    long chpars[] = {AL_OUTPUT_RATE, 0};
    if (cfg.frequency > 48000)
        chpars[1] = AL_RATE_48000;
    else if (cfg.frequency > 44100)
        chpars[1] = AL_RATE_44100;
    else if (cfg.frequency > 32000)
        chpars[1] = AL_RATE_32000;
    else if (cfg.frequency > 22050)
        chpars[1] = AL_RATE_22050;
    else if (cfg.frequency > 16000)
        chpars[1] = AL_RATE_16000;
    else
        chpars[1] = AL_RATE_11025;
    ALsetparams(AL_DEFAULT_DEVICE, chpars, 2);
    ALgetparams(AL_DEFAULT_DEVICE, chpars, 2);
    _settings.frequency = (uint_least32_t) chpars[1];

    // Allocate sound buffers and set audio queue
    ALsetqueuesize(_config, chpars[1]);

    // open audio device
    _audio = ALopenport("SIDPLAY2 sound", "w", _config);
    if (_audio == NULL)
    {
        perror("AUDIO:");
        _errorString = "ERROR: Could not open audio device.\n       See standard error output.";
        ALfreeconfig(_config);
        return 0;
    }

    // Setup internal Config
    _settings.encoding  = AUDIO_SIGNED_PCM;
    _settings.bufSize   = (uint_least32_t) chpars[1];

    // Update the users settings
    getConfig (cfg);

    // Allocate memory same size as buffer
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) int_least8_t[chpars[1]];
#else
    _sampleBuffer = new int_least8_t[chpars[1]];
#endif

    _errorString = "OK";
    return (void *) _sampleBuffer;
}

void *Audio_Irix::reset()
{
    // Flush output stream.
    if (_audio != NULL)
    {
        return _sampleBuffer;
    }
    return NULL;
}

void Audio_Irix::close ()
{
    if (_audio != NULL)
    {
        ALcloseport(_audio);
        ALfreeconfig(_config);
        _audio  = NULL;
        _config = NULL;
        outOfOrder ();
    }
}

void *Audio_Irix::write ()
{
    if (_audio != NULL)
    {
        ALwritesamps(_audio, (char*)_sampleBuffer, _settings.bufSize);
        return _sampleBuffer;
    }

    _errorString = "ERROR: Device not open.";
    return 0;
}

#endif // HAVE_IRIX
