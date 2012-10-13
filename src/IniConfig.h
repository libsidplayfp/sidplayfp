/***************************************************************************
                          IniConfig.h  -  Sidplay2 config file reader.
                             -------------------
    begin                : Sun Mar 25 2001
    copyright            : (C) 2000 by Simon White
    email                : s_a_white@email.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _IniConfig_h_
#define _IniConfig_h_

#include "ini/libini.h"

#include <sidplayfp/SidConfig.h>

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
        SidConfig::clock_t  clockSpeed;
        bool          clockForced;
        SidConfig::model_t  sidModel;
        bool          forceModel;
        bool          filter;
        double        bias;
        double        filterCurve6581;
        int           filterCurve8580;
        uint_least8_t optimiseLevel;
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

    // Sidplay2 Specific Section
    const sidplay2_section&  sidplay2     () { return sidplay2_s; }
    const console_section&   console      () { return console_s; }
    const audio_section&     audio        () { return audio_s; }
    const emulation_section& emulation    () { return emulation_s; }
};

#endif // _IniConfig_h_
