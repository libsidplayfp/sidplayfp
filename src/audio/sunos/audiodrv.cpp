/***************************************************************************
                          audiodrv.cpp  -  SunOS sound support
                             -------------------
    begin                : Sat Jul 8 2000
    copyright            : (C) 2000 by Simon White
    email                : s_a_white@email.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *  $Log: audiodrv.cpp,v $
 *  Revision 1.5  2002/03/04 19:07:48  s_a_white
 *  Fix C++ use of nothrow.
 *
 *  Revision 1.4  2002/01/10 19:04:01  s_a_white
 *  Interface changes for audio drivers.
 *
 *  Revision 1.3  2001/12/11 19:38:13  s_a_white
 *  More GCC3 Fixes.
 *
 *  Revision 1.2  2001/01/18 18:36:16  s_a_white
 *  Support for multiple drivers added.  C standard update applied (There
 *  should be no spaces before #)
 *
 *  Revision 1.1  2001/01/08 16:41:43  s_a_white
 *  App and Library Seperation
 *
 *  Revision 1.5  2000/12/11 19:08:32  s_a_white
 *  AC99 Update.
 *
 ***************************************************************************/

// --------------------------------------------------------------------------
// SPARCstation specific audio interface. (very poor)
// --------------------------------------------------------------------------

#include "audiodrv.h"
#ifdef   HAVE_SUNOS

#ifdef HAVE_EXCEPTIONS
#   include <new>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
// for streamio ioctl commands and args
#include <sys/types.h>
#include <stropts.h>
#include <sys/conf.h>

#if defined(HAVE_SUN_AUDIOIO_H)
#   include <sun/audioio.h>
//#   include <sun/dbriio.h>
#elif defined(HAVE_SYS_AUDIOIO_H)
#   include <sys/audioio.h>
#else
#   error Audio driver not supported.
#endif

const char Audio_SunOS::AUDIODEVICE[] = "/dev/audio";


Audio_SunOS::Audio_SunOS()
{
    outOfOrder();
}

Audio_SunOS::~Audio_SunOS()
{
    close();
}

void Audio_SunOS::outOfOrder()
{
    // Reset everything.
    _errorString = "None";
    _audiofd     = (-1);
}

void *Audio_SunOS::open (AudioConfig& cfg, const char *)
{
    if ((_audiofd =::open (AUDIODEVICE,O_WRONLY,0)) == (-1))
    {
        perror (AUDIODEVICE);
        _errorString = "ERROR: Could not open audio device.\n       See standard error output.";
        return 0;
    }

#ifdef CHECKFORSPEAKERBOX
    int hwdevice;
    if (ioctl (_audiofd, AUDIO_GETDEV, &hwdevice) == (-1))
    {
        perror (AUDIODEVICE);
        _errorString = "AUDIO: No audio hardware device installed.";
        return 0;
    }
    if (hwdevice != AUDIO_DEV_SPEAKERBOX)
    {
        _audiofd = -1;
        perror (AUDIODEVICE);
        _errorString = "AUDIO: Speakerbox not installed/enabled.";
        return 0;
    }
#endif
	
    // Choose the nearest possible frequency.
    int dbrifreqs[] =
    {
        8000, 9600, 11025, 16000, 18900, 22050, 32000, 37800, 44100, 48000, 0
    };
    int dbrifsel      = 0;
    int dbrifreqdiff  = 100000;
    int frequency     = (int) cfg.frequency;
    int dbrifrequency = frequency;
    do
    {
        int dbrifreqdiff2 = frequency  - dbrifreqs[dbrifsel];
        dbrifreqdiff2 < 0 ? dbrifreqdiff2 = 0 - dbrifreqdiff2 : dbrifreqdiff2 += 0;
        if (dbrifreqdiff2 < dbrifreqdiff)
        {
            dbrifreqdiff  = dbrifreqdiff2;
            dbrifrequency = dbrifreqs[dbrifsel];
        }
        dbrifsel++;
    }  while ( dbrifreqs[dbrifsel] != 0 );
    _settings.frequency = dbrifrequency;
	
    audio_info myaudio_info;
    if (ioctl (_audiofd, AUDIO_GETINFO, &myaudio_info) == (-1))
    {
        perror (AUDIODEVICE);
        _errorString = "AUDIO: Could not get audio info.\n       See standard error output.";
        return 0;
    }
    AUDIO_INITINFO( &myaudio_info );

    myaudio_info.play.sample_rate = (uint_t) cfg.frequency;
    myaudio_info.play.channels    = cfg.channels;
    // Only poor audio capabilities at 8-bit.
    // Sparcstations 5 and 10 tend to be 16-bit only at rates above 8000 Hz.
    myaudio_info.play.precision   = 16;
    myaudio_info.play.encoding    = AUDIO_ENCODING_LINEAR;
    myaudio_info.output_muted     = 0;
    if (ioctl (_audiofd,AUDIO_SETINFO,&myaudio_info) == (-1))
    {
        perror (AUDIODEVICE);
        _errorString = "AUDIO: Could not set audio info.\n       See standard error output.";
        return 0;
    }

    // Setup internal Config
    cfg.frequency = myaudio_info.play.sample_rate;
    cfg.channels  = myaudio_info.play.channels;
    cfg.encoding  = AUDIO_SIGNED_PCM;
    cfg.bufSize   = myaudio_info.play.buffer_size;
    cfg.precision = myaudio_info.play.precision;

    // Copy input parameters. May later be replaced with driver defaults.
    _settings = cfg;

    // Allocate memory same size as buffer
#ifdef HAVE_EXCEPTIONS
    _sampleBuffer = new(std::nothrow) int_least8_t[myaudio_info.play.buffer_size];
#else
    _sampleBuffer = new int_least8_t[myaudio_info.play.buffer_size];
#endif

    _errorString = "OK";
    return _sampleBuffer;
}

void *Audio_SunOS::reset()
{
    // Flush output stream.
    if (_audiofd != (-1))
    {
        if (ioctl (_audiofd, I_FLUSH, FLUSHW) != (-1))
            return _sampleBuffer;
    }
    return NULL;
}

void Audio_SunOS::close ()
{
    if (_audiofd != (-1))
    {
        ::close (_audiofd);
        delete [] (int_least32_t *) _sampleBuffer;         
        outOfOrder ();
    }
}

void *Audio_SunOS::write ()
{
    if (_audiofd != (-1))
    {
        ::write (_audiofd, (char*) _sampleBuffer, _settings.bufSize);
        return _sampleBuffer;
    }

    _errorString = "ERROR: Device not open.";
    return 0;
}

#endif // HAVE_SUNOS
