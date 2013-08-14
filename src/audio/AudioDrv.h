/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2000 Simon White
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

#ifndef AUDIODRV_H
#define AUDIODRV_H

// Drivers must be put in order of preference
#include "config.h"

// Hardsid Compatibility Driver
#include "null/null.h"

// Unix Sound Drivers
#include "pulse/audiodrv.h"
#include "alsa/audiodrv.h"
#include "oss/audiodrv.h"

// Windows Sound Drivers
#include "directx/directx.h"
#include "mmsystem/mmsystem.h"

// Make sure that a sound driver was used
#ifndef AudioDriver
#  warning Audio hardware not recognised, please check configuration files.
#endif

// Add music conversion drivers
#include "wav/WavFile.h"

#include <memory>

class audioDrv : public AudioBase
{
private:
    std::auto_ptr<AudioBase> audio;

public:
    audioDrv() : audio(new AudioDriver()) {}
    virtual ~audioDrv() {}

    bool open(AudioConfig &cfg, const char *name) { audio->open(cfg, name); }
    void reset() { audio->reset(); }
    bool write() { return audio->write(); }
    void close() { audio->close(); }
    void pause() { audio->pause(); }
    const char *extension() const { return audio->extension(); }
};

#endif // AUDIODRV_H
