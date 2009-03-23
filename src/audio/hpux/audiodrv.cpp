// --------------------------------------------------------------------------
// HPPA/HPUX specific audio interface. (very poor)
// --------------------------------------------------------------------------
/***************************************************************************
 *  $Log: audiodrv.cpp,v $
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
#ifdef   HAVE_HPUX

#ifdef HAVE_EXCEPTIONS
#   include <new>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#if defined(HAVE_SYS_AUDIO_H)
#   include <sys/audio.h>
#else
#   error Audio driver not supported.
#endif

const char Audio_HPUX::AUDIODEVICE[] = "/dev/audio";

Audio_HPUX::Audio_HPUX()
{
    outOfOrder();
}

Audio_HPUX::~Audio_HPUX()
{
    close();
}

void Audio_HPUX::outOfOrder()
{
    // Reset everything.
    _errorString = "None";
    _audiofd     = (-1);
}

void *Audio_HPUX::open (AudioConfig& cfg, const char *)
{
    // Copy input parameters. May later be replaced with driver defaults.
    _settings = cfg;

    if ((_audiofd =::open (AUDIODEVICE,O_WRONLY,0)) == (-1))
    {
        perror (AUDIODEVICE);
        _errorString = "ERROR: Could not open audio device.\n       See standard error output.";
        return 0;
    }

    // Choose the nearest possible frequency.
    int dbrifreqs[] =
    {
      5512, 6615, 8000, 9600, 11025, 16000, 18900, 22050, 27428, 32000,
      44100, 48000, 0
    };
    int dbrifsel      = 0;
    int dbrifreqdiff  = 100000;
    int dbrifrequency = _settings.frequency;
    do
    {
        int dbrifreqdiff2 = _settings.frequency  - dbrifreqs[dbrifsel];
        dbrifreqdiff2 < 0 ? dbrifreqdiff2 = 0 - dbrifreqdiff2 : dbrifreqdiff2 += 0;
        if (dbrifreqdiff2 < dbrifreqdiff)
        {
            dbrifreqdiff  = dbrifreqdiff2;
            dbrifrequency = dbrifreqs[dbrifsel];
        }
        dbrifsel++;
    }  while ( dbrifreqs[dbrifsel] != 0 );

    _settings.frequency = dbrifrequency;

    if ( ( ioctl(_audiofd, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT ) ) < 0)
    {
        perror (AUDIODEVICE);
        _errorString = "ERROR: Could not set sample format.\n       See standard error output.";
        goto open_error;
    }

    if (ioctl(_audiofd, AUDIO_SET_CHANNELS, _settings.channels) < 0 )
    {
        perror (AUDIODEVICE);
        _errorString = "ERROR: Could not set mono/stereo.\n       See standard error output.";
        goto open_error;
    } 

    if (ioctl(_audiofd, AUDIO_SET_SAMPLE_RATE,_settings.frequency)< 0)
    {
        perror (AUDIODEVICE);
        _errorString = "ERROR: Could not set sample rate.\n       See standard error output.";
        goto open_error;
   } 
 
    // Setup internal Config
    _settings.encoding  = AUDIO_SIGNED_PCM;
    _settings.bufSize   = _settings.frequency;
    _settings.precision = 16; // No other modes supported by the HW
    // Update the users settings
    getConfig (cfg);

    // Allocate memory same size as buffer
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) int_least8_t[_settings.bufSize];
#else
    _sampleBuffer = new int_least8_t[_settings.bufSize];
#endif

    _errorString = "OK";
    return (void *) _sampleBuffer;

open_error:
    ::close(_audiofd);
    _audiofd = (-1);
    return 0;
}

void *Audio_HPUX::reset()
{
    // Flush output stream.
    if (_audiofd != (-1))
    {
        return _sampleBuffer;
    }
    return NULL;
}

void Audio_HPUX::close ()
{
    if (_audiofd != (-1))
    {
        ::close (_audiofd);
        outOfOrder ();
    }
}

void *Audio_HPUX::write ()
{
    if (_audiofd != (-1))
    {
        ::write (_audiofd, (char*) _sampleBuffer, _settings.bufSize);
        return _sampleBuffer;
    }

    _errorString = "ERROR: Device not open.";
    return 0;
}

#endif // HAVE_HPUX
