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

#include "utils.h"

#include <cstdlib>

#ifdef _WIN32
#  include <shlobj.h>
#  include <shlwapi.h>

#ifdef UNICODE
#  define _tgetenv_s _wgetenv_s
#else
#  define _tgetenv_s getenv_s
#endif

std::wstring utils::utf8_decode(const char *str)
{
    if (!str)
        return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    std::wstring strTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &strTo[0], size_needed);
    return strTo;
}

std::string utils::utf8_encode(const TCHAR *wstr)
{
    if (!wstr)
        return std::string();
#ifdef UNICODE
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, NULL, NULL);
    return strTo;
#else
    return std::string(wstr);
#endif
}

std::string utils::getExecPath()
{
    HMODULE hModule = GetModuleHandle(NULL);
    TCHAR path[MAX_PATH];
    GetModuleFileName(hModule, path, MAX_PATH);
    PathRemoveFileSpec(path);
    return utf8_encode(path);
}

std::string utils::getPath()
{
    std::string returnPath;

    TCHAR szPath[MAX_PATH];

    if (SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, szPath)!=S_OK)
    {
        size_t pReturnValue;
        errno_t res = _tgetenv_s(&pReturnValue, szPath, TEXT("USERPROFILE"));
        if (res != 0)
            throw error();
        returnPath.append(utf8_encode(szPath)).append("\\Application Data");
    }
    else
    {
        returnPath.append(utf8_encode(szPath));
    }

    return returnPath;
}

std::string utils::getDataPath() { return getPath(); }

std::string utils::getConfigPath() { return getPath(); }

#else

std::string utils::getPath(const char* id, const char* def)
{
    std::string returnPath;

    char *path = std::getenv(id);
    if (!path)
    {
        path = std::getenv("HOME");
        if (!path)
            throw error();
        returnPath.append(path).append(def);
    }
    else
        returnPath.append(path);

    return returnPath;
}

std::string utils::getDataPath() { return getPath("XDG_DATA_HOME", "/.local/share"); }

std::string utils::getConfigPath() { return getPath("XDG_CONFIG_HOME", "/.config"); }

#endif
