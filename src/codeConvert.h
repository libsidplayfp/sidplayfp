/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2021 Leandro Nini
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

#ifndef CODECONVERT_H
#define CODECONVERT_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef HAVE_ICONV
#  include <clocale>
#  include <iconv.h>
#ifndef _WIN32
#  include <langinfo.h>
#else
#  include <windows.h>
#endif
#endif

class codeConvert
{
private:
#ifdef HAVE_ICONV
    iconv_t cd;

    char buffer[128];
#endif

public:
    codeConvert();
    ~codeConvert();

    const char* convert(const char* src);
};


#endif
