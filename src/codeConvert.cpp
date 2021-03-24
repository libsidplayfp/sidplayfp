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

#include "codeConvert.h"

#ifdef HAVE_ICONV
#  include "codepages.h"
#endif

const char* codeConvert::convert(const char* src)
{
#ifdef HAVE_ICONV
    if (cd == (iconv_t) -1)
        return src;

    char *srcPtr = const_cast<char*>(src);
    size_t srcLeft = strlen(src);
    char *outPtr = buffer;
    size_t outLeft = 128;

    while (srcLeft > 0)
    {
        size_t ret = iconv(cd, &srcPtr, &srcLeft, &outPtr, &outLeft);
        if (ret == (size_t) -1)
            break;
    }

    return buffer;
#else
    return src;
#endif
}

codeConvert::codeConvert()
{
#ifdef HAVE_ICONV
    setlocale(LC_ALL, "");
    const char* encoding;
#  ifndef _WIN32
    encoding = nl_langinfo(CODESET);
#  else
    UINT codepage = GetConsoleOutputCP();
    //CPINFOEX cpinfo;
    //GetCPInfoEx(codepage, 0, &cpinfo);

    encoding = codepageName(codepage);
#  endif

    cd = iconv_open(encoding, "ISO-8859-1");
#endif
}

codeConvert::~codeConvert()
{
#ifdef HAVE_ICONV
    iconv_close(cd);
#endif
}
