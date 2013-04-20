/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2000 Jarno Paananen
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

#ifndef _audio_mmsystem_h_
#define _audio_mmsystem_h_

#include "config.h"
#ifdef   HAVE_MMSYSTEM

#ifndef AudioDriver
#define AudioDriver Audio_MMSystem
#endif

#include <windows.h>
#include <mmsystem.h>
#include "../AudioBase.h"


class Audio_MMSystem: public AudioBase
{
private:  // ------------------------------------------------------- private
    HWAVEOUT    waveHandle;

    // Rev 1.3 (saw) - Buffer sizes adjusted to get a
    // correct playtimes
    #define  MAXBUFBLOCKS 3
    short   *blocks[MAXBUFBLOCKS];
    HGLOBAL  blockHandles[MAXBUFBLOCKS];
    WAVEHDR *blockHeaders[MAXBUFBLOCKS];
    HGLOBAL  blockHeaderHandles[MAXBUFBLOCKS];
    int      blockNum;
    bool     isOpen;
    int      bufSize;

public:  // --------------------------------------------------------- public
    Audio_MMSystem();
    ~Audio_MMSystem();

    short *open  (AudioConfig &cfg, const char *name);
    void  close ();
    // Rev 1.2 (saw) - Changed, see AudioBase.h
    short *reset ();
    short *write ();
    void  pause () {;}
};

#endif // HAVE_MMSYSTEM
#endif // _audio_mmsystem_h_
