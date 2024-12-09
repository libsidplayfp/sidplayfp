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

#include "mixer.h"

void Mixer::begin(short *buffer, uint_least32_t length)
{
    m_dest = buffer;
    m_dest_size = length;

    m_pos = m_buffer.size();
    for (uint_least32_t i=0; i<m_pos; i++) // FIXME use memcpy
    {
        m_dest[i] = m_buffer[i];
    }
}

void Mixer::doMix(short* (&buffers)[], uint_least32_t samples)
{
    uint_least32_t const cnt = std::min(samples, m_dest_size-m_pos);
    for (uint_least32_t i=0; i<cnt; i++)
    {
        m_dest[m_pos+i] = buffers[0][i];
    }
    m_pos += cnt;

    // save remaining samples, if any
    uint_least32_t const rem = samples - cnt;
    m_buffer.resize(rem);
    for (uint_least32_t i=0; i<rem; i++) // FIXME use memcpy
    {
        m_buffer[i] = buffers[0][cnt+i];
    }
}
