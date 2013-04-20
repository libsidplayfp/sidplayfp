/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2008 Antti Lankila
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

#ifndef audio_pulse_h_
#define audio_pulse_h_

#include "config.h"
#ifdef   HAVE_PULSE

#ifndef AudioDriver
#define AudioDriver Audio_Pulse
#endif

#include <pulse/simple.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "../AudioBase.h"

class Audio_Pulse: public AudioBase
{
private:  // ------------------------------------------------------- private
    pa_simple *_audioHandle;
    void outOfOrder ();

public:  // --------------------------------------------------------- public
    Audio_Pulse();
    ~Audio_Pulse();

    short *open  (AudioConfig &cfg, const char *name);
    void  close ();
    short *reset ()
    {
        return _sampleBuffer;
    }
    short *write ();
    void  pause () {;}
};

#endif // HAVE_PULSE
#endif // audio_pulse_h_
