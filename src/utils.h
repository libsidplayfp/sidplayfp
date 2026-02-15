/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2013-2026 Leandro Nini
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

#ifndef UTILS_H
#define UTILS_H

#include <string>

#ifdef _WIN32
#  include <windows.h>
#endif

class utils
{
public:
    class error {};

private:
#ifdef _WIN32
    static std::string getPath();
#else
    static std::string getPath(const char* id, const char* def);
#endif

public:
    /**
     * Get the system path for data files.
     */
    static std::string getDataPath();

    /**
     * Get the system path for config files.
     */
    static std::string getConfigPath();

#ifdef _WIN32
    /**
     * Get the path of the executable.
     */
    static std::string getExecPath();

    /**
     * Convert from UTF8 to UTF16.
     */
    static std::wstring utf8_decode(const char *str);

    /**
     * Convert from UTF16 to UTF8.
     */
    static std::string utf8_encode(const TCHAR *wstr);
#endif
};

#endif
