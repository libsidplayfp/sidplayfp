/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2000-2001 Simon White
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

#ifndef audio_null_h_
#define audio_null_h_

#include "config.h"
#ifdef   HAVE_HARDSID
#   ifndef AudioDriver
#   define AudioDriver Audio_Null
#   endif
#endif

#include "../AudioBase.h"

/*
 * Null audio driver used for hardsid
 * and songlength detection
 */
class Audio_Null: public AudioBase
{
private:  // ------------------------------------------------------- private
    bool isOpen;

public:  // --------------------------------------------------------- public
    Audio_Null();
    ~Audio_Null();

    short *open  (AudioConfig &cfg, const char *);
    void  close ();
    short *reset ();
    short *write ();
    void  pause () {;}
};

#endif // audio_null_h_
