/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2000-2005 Simon White
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

#ifndef AUDIO_ALSA_H
#define AUDIO_ALSA_H

#include "config.h"
#ifdef   HAVE_ALSA

#ifndef AudioDriver
#define AudioDriver Audio_ALSA
#endif

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>
#include "../AudioBase.h"


class Audio_ALSA: public AudioBase
{
private:  // ------------------------------------------------------- private
    snd_pcm_t * _audioHandle;
    int _alsa_to_frames_divisor;

    void outOfOrder ();

public:  // --------------------------------------------------------- public
    Audio_ALSA();
    ~Audio_ALSA();

    short *open  (AudioConfig &cfg, const char *name);
    void  close ();
    // Rev 1.2 (saw) - Changed, see AudioBase.h
    short *reset ();
    short *write ();
    void  pause () {;}
};

#endif // HAVE_ALSA
#endif // AUDIO_ALSA_H
