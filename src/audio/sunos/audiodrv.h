/***************************************************************************
                          audiodrv.h  -  SunOS sound support
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
 *  $Log: audiodrv.h,v $
 *  Revision 1.5  2002/01/10 19:04:01  s_a_white
 *  Interface changes for audio drivers.
 *
 *  Revision 1.4  2001/10/30 23:35:35  s_a_white
 *  Added pause support.
 *
 *  Revision 1.3  2001/01/23 17:50:59  s_a_white
 *  Removed duplicate #endif.
 *
 *  Revision 1.2  2001/01/18 18:36:16  s_a_white
 *  Support for multiple drivers added.  C standard update applied (There
 *  should be no spaces before #)
 *
 *  Revision 1.1  2001/01/08 16:41:43  s_a_white
 *  App and Library Seperation
 *
 *  Revision 1.4  2000/12/11 19:08:32  s_a_white
 *  AC99 Update.
 *
 ***************************************************************************/

// --------------------------------------------------------------------------
// SPARCstation specific audio interface.
// --------------------------------------------------------------------------

#ifndef audio_sunos_h_
#define audio_sunos_h_

#include "config.h"
#ifdef   HAVE_SUNOS

#ifndef AudioDriver
#define AudioDriver Audio_SunOS
#endif

#include "../AudioBase.h"


class Audio_SunOS: public AudioBase
{
private:  // ------------------------------------------------------- private
    static const char AUDIODEVICE[];
    void   outOfOrder ();
    int    _audiofd;

public:  // --------------------------------------------------------- public
    Audio_SunOS();
    ~Audio_SunOS();

    void *open (AudioConfig &cfg, const char *name);
	
    // Free and close opened audio device and reset any variables that
    // reflect the current state of the driver.
    void close();
	
    // Flush output stream.
    // Rev 1.3 (saw) - Changed, see AudioBase.h	
    void *reset ();
    void *write ();		
    void  pause () {;}
};

#endif // HAVE_SUNOS
#endif // audio_sunos_h_
