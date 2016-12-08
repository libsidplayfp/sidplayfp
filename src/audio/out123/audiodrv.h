/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2016 Leandro Nini
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

#ifndef AUDIO_OUT123_H
#define AUDIO_OUT123_H

#ifndef AudioDriver
#  define AudioDriver Audio_OUT123
#endif

#include <out123.h>

#include "../AudioBase.h"

/*
 * Open Sound System (OSS) specific audio driver interface.
 */
class Audio_OUT123: public AudioBase
{
private:  // ------------------------------------------------------- private
    out123_handle *_audiofd;

    void outOfOrder ();

public:  // --------------------------------------------------------- public
    Audio_OUT123();
    ~Audio_OUT123();

    bool open  (AudioConfig &cfg) override;
    void close () override;
    void reset () override;
    bool write () override;
    void pause () override {}
};

#endif // AUDIO_OUT123_H
