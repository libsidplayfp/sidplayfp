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

#ifndef AUDIOCONFIG_H
#define AUDIOCONFIG_H

#include <stdint.h>

class AudioConfig
{
public:
    uint_least32_t frequency;
    int            precision;
    int            channels;
    uint_least32_t bufSize;       // sample buffer size

    AudioConfig() :
        frequency(48000),
        precision(16),
        channels(1),
        bufSize(0) {}

    uint_least32_t bytesPerMillis() const { return (precision/8 * channels * frequency) / 1000; }
};

#endif  // AUDIOCONFIG_H
