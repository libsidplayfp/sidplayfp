/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2021-2026 Leandro Nini
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

#include "codeConvert.h"

#include <cstring>


const char* codeConvert::convert(const char* src)
{
    // keep room for a two byte sequence and the terminator
    const unsigned int limit = sizeof(buffer) - 3;
    unsigned int i=0; // source index
    unsigned int j=0; // destination index
    while (src[i] && (j <= limit))
    {
        unsigned char ch = static_cast<unsigned char>(src[i++]);
        if (ch < 0x80)
            buffer[j++] = static_cast<char>(ch);
        else if (ch <= 0xBF)
        {
            buffer[j++] = static_cast<char>(0xC2);
            buffer[j++] = static_cast<char>(ch);
        }
        else
        {
            buffer[j++] = static_cast<char>(0xC3);
            buffer[j++] = static_cast<char>(ch - 0x40);
        }
    }

    // terminate buffer string
    buffer[j] = 0;

    return buffer;
}
