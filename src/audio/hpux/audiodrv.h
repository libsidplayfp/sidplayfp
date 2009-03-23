// --------------------------------------------------------------------------
// HPPA/HPUX specific audio interface.
// --------------------------------------------------------------------------
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
 ***************************************************************************/

#ifndef audio_hpux_h_
#define audio_hpux_h_

#include "config.h"
#ifdef   HAVE_HPUX

#ifndef AudioDriver
#define AudioDriver Audio_HPUX
#endif

#include "../AudioBase.h"


class Audio_HPUX: public AudioBase
{
private:  // ------------------------------------------------------- private
    static const char AUDIODEVICE[];
    void   outOfOrder ();
    int    _audiofd;

public:  // --------------------------------------------------------- public
    Audio_HPUX();
    ~Audio_HPUX();

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

#endif // HAVE_HPUX
#endif // audio_hpux_h_
