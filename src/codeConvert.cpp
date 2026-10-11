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


namespace
{
    // Windows-1252 0x80-0x9F as Unicode code points
    // undefined bytes (0x81, 0x8D, 0x8F, 0x90, 0x9D) map to the C1 control
    // of the same value (U+0081 etc.), the same as Latin-1
    const unsigned short cp1252[32] =
    {
        0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
        0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
        0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178
    };
}

const char* codeConvert::convert(const char* src)
{
    // keep room for a three byte sequence and the terminator
    const unsigned int limit = sizeof(buffer) - 4;
    unsigned int i=0; // source index
    unsigned int j=0; // destination index
    while (src[i] && (j <= limit))
    {
        unsigned char ch = static_cast<unsigned char>(src[i++]);
        if (ch < 0x80)
            buffer[j++] = static_cast<char>(ch);
        else if (ch <= 0x9F)
        {
            const unsigned int cp = cp1252[ch - 0x80];
            if (cp < 0x800)
            {
                buffer[j++] = static_cast<char>(0xC0 | (cp >> 6));
                buffer[j++] = static_cast<char>(0x80 | (cp & 0x3F));
            }
            else
            {
                buffer[j++] = static_cast<char>(0xE0 | (cp >> 12));
                buffer[j++] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                buffer[j++] = static_cast<char>(0x80 | (cp & 0x3F));
            }
        }
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
