/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2011-2012 Leandro Nini
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

#ifndef INICONFIG_H
#define INICONFIG_H

#include "ini/libini.h"

#include <sidplayfp/SidConfig.h>

/*
 * Sidplayfp config file reader.
 */
class IniConfig
{
public:
    struct sidplay2_section
    {
        int            version;
        char          *database;
        uint_least32_t playLength;
        uint_least32_t recordLength;
        char          *kernalRom;
        char          *basicRom;
        char          *chargenRom;
    };

    struct console_section
    {   // INI Section - [Console]
        bool ansi;
        char topLeft;
        char topRight;
        char bottomLeft;
        char bottomRight;
        char vertical;
        char horizontal;
        char junctionLeft;
        char junctionRight;
    };

    struct audio_section
    {   // INI Section - [Audio]
        long frequency;
        SidConfig::playback_t playback;
        int  precision;
    };

    struct emulation_section
    {   // INI Section - [Emulation]
        SidConfig::c64_model_t  modelDefault;
        bool          modelForced;
        SidConfig::sid_model_t  sidModel;
        bool          forceModel;
        bool          filter;
        double        bias;
        double        filterCurve6581;
        int           filterCurve8580;
    };

protected:
    static const char *DIR_NAME;
    static const char *FILE_NAME;

    bool      status;
    struct    sidplay2_section  sidplay2_s;
    struct    console_section   console_s;
    struct    audio_section     audio_s;
    struct    emulation_section emulation_s;

protected:
    void  clear ();

    bool  readInt    (ini_fd_t ini, const char *key, int &value);
    bool  readDouble (ini_fd_t ini, const char *key, double &value);
    bool  readString (ini_fd_t ini, const char *key, char *&str);
    bool  readBool   (ini_fd_t ini, const char *key, bool &boolean);
    bool  readChar   (ini_fd_t ini, const char *key, char &ch);
    bool  readTime   (ini_fd_t ini, const char *key, int  &time);

    bool  readSidplay2  (ini_fd_t ini);
    bool  readConsole   (ini_fd_t ini);
    bool  readAudio     (ini_fd_t ini);
    bool  readEmulation (ini_fd_t ini);

public:
    IniConfig  ();
    ~IniConfig ();

    void read ();
    operator bool () { return status; }

    // Sidplayfp Specific Section
    const sidplay2_section&  sidplay2     () { return sidplay2_s; }
    const console_section&   console      () { return console_s; }
    const audio_section&     audio        () { return audio_s; }
    const emulation_section& emulation    () { return emulation_s; }
};

#endif // INICONFIG_H
