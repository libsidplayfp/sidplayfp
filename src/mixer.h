/*
 * This file is part of sidplayfp, a SID player engine.
 *
 * Copyright 2011-2024 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright (C) 2000 Simon White
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>

#include <vector>

/**
 * This class implements the mixer.
 */
class Mixer
{
private:
    uint_least32_t m_pos = 0;
    uint_least32_t m_dest_size = 0;

    short* m_dest = nullptr;

    int m_channels = 1;

    std::vector<short> m_buffer;

public:
    void begin(short *buffer, uint_least32_t length);

    void doMix(short** buffers, uint_least32_t samples);

    bool isFull() const { return m_pos >= m_dest_size; }

    void clear() { m_buffer.resize(0); }
};

#endif // MIXER_H
