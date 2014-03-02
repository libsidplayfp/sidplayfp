/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2011-2013 Leandro Nini
 * Copyright 2000-2001 Simon White
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "IniConfig.h"

#include <string>

#include <string.h>
#include <stdlib.h>

#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/stat.h>  /* mkdir */
#  include <dirent.h>    /* opendir */
#else
#  include <windows.h>
#endif

#include "utils.h"

const char *IniConfig::DIR_NAME  = "sidplayfp";
const char *IniConfig::FILE_NAME = "sidplayfp.ini";

#define SAFE_FREE(p) { if(p) { free (p); (p)=NULL; } }

IniConfig::IniConfig () :
    status(true)
{   // Initialise everything else
    sidplay2_s.database    = NULL;
    sidplay2_s.kernalRom   = NULL;
    sidplay2_s.basicRom    = NULL;
    sidplay2_s.chargenRom  = NULL;
    clear ();
}


IniConfig::~IniConfig ()
{
    clear ();
}


void IniConfig::clear ()
{
    sidplay2_s.version      = 1;           // INI File Version
    SAFE_FREE (sidplay2_s.database);
    sidplay2_s.playLength   = 0;           // INFINITE
    sidplay2_s.recordLength = 3 * 60 + 30; // 3.5 minutes
    SAFE_FREE (sidplay2_s.kernalRom);
    SAFE_FREE (sidplay2_s.basicRom);
    SAFE_FREE (sidplay2_s.chargenRom);

    console_s.ansi          = false;
    console_s.topLeft       = '+';
    console_s.topRight      = '+';
    console_s.bottomLeft    = '+';
    console_s.bottomRight   = '+';
    console_s.vertical      = '|';
    console_s.horizontal    = '-';
    console_s.junctionLeft  = '+';
    console_s.junctionRight = '+';

    audio_s.frequency = SidConfig::DEFAULT_SAMPLING_FREQ;
    audio_s.playback  = SidConfig::MONO;
    audio_s.precision = 16;

    emulation_s.modelDefault  = SidConfig::PAL;
    emulation_s.modelForced   = false;
    emulation_s.sidModel      = SidConfig::MOS6581;
    emulation_s.forceModel    = false;
    emulation_s.filter        = true;

    emulation_s.bias            = 0.0;
    emulation_s.filterCurve6581 = 0.0;
    emulation_s.filterCurve8580 = 0;
}


bool  IniConfig::readDouble (ini_fd_t ini, const char *key, double &value)
{
    double d = value;
    if (ini_locateKey (ini, key) < 0)
    {   // Doesn't exist, add it
        (void) ini_writeString (ini, "");
    }
    if (ini_readDouble (ini, &d) < 0)
        return false;
    value = d;
    return true;
}


bool IniConfig::readInt (ini_fd_t ini, const char *key, int &value)
{
    int i = value;
    if (ini_locateKey (ini, key) < 0)
    {   // Doesn't exist, add it
        (void) ini_writeString (ini, "");
    }
    if (ini_readInt (ini, &i) < 0)
        return false;
    value = i;
    return true;
}


bool IniConfig::readString (ini_fd_t ini, const char *key, char *&str)
{
    if (ini_locateKey (ini, key) < 0)
    {   // Doesn't exist, add it
        (void) ini_writeString (ini, "");
    }

    size_t length = (size_t) ini_dataLength (ini);
    if (!length)
        return false;

    char *ret = (char *) malloc (++length);
    if (!ret)
        return false;

    if (ini_readString (ini, ret, (unsigned int) length) < 0)
    {
        free (ret);
        return false;
    }

    str = ret;
    return true;
}


bool IniConfig::readBool (ini_fd_t ini, const char *key, bool &boolean)
{
    int b = boolean;
    if (ini_locateKey (ini, key) < 0)
    {   // Doesn't exist, add it
        (void) ini_writeString (ini, "");
    }
    if (ini_readBool (ini, &b) < 0)
        return false;
    boolean = (b != 0);
    return true;
}


bool IniConfig::readChar (ini_fd_t ini, const char *key, char &ch)
{
    char *str, c = 0;
    bool  ret = readString (ini, key, str);
    if (!ret)
        return false;

    // Check if we have an actual chanracter
    if (str[0] == '\'')
    {
        if (str[2] != '\'')
            ret = false;
        else
            c = str[1];
    } // Nope is number
    else
        c = (char) atoi (str);

    // Clip off special characters
    if ((unsigned) c >= 32)
        ch = c;

    free (str);
    return ret;
}


bool IniConfig::readTime (ini_fd_t ini, const char *key, int &value)
{
    char *str;
    bool  ret = readString (ini, key, str);
    if (!ret)
        return false;

    if (!*str)
        return false;

    int time;
    char *sep = strstr (str, ":");
    if (!sep)
    {   // User gave seconds
        time = atoi (str);
    }
    else
    {   // Read in MM:SS format
        *sep = '\0';
        int val = atoi (str);
        if (val < 0 || val > 99)
            goto IniCofig_readTime_error;
        time = val * 60;
        val  = atoi (sep + 1);
        if (val < 0 || val > 59)
            goto IniCofig_readTime_error;
        time += val;
    }

    value = time;
    free (str);
    return ret;

IniCofig_readTime_error:
    free (str);
    return false;
}


bool IniConfig::readSidplay2 (ini_fd_t ini)
{
    bool ret = true;

    (void) ini_locateHeading (ini, "SIDPlayfp");

    int version = sidplay2_s.version;
    ret &= readInt (ini, "Version", version);
    if (version > 0)
        sidplay2_s.version = version;

    ret &= readString (ini, "Songlength Database", sidplay2_s.database);

    int time;
    if (readTime (ini, "Default Play Length", time))
        sidplay2_s.playLength   = (uint_least32_t) time;
    if (readTime (ini, "Default Record Length", time))
        sidplay2_s.recordLength = (uint_least32_t) time;

    ret &= readString (ini, "Kernal Rom", sidplay2_s.kernalRom);
    ret &= readString (ini, "Basic Rom", sidplay2_s.basicRom);
    ret &= readString (ini, "Chargen Rom", sidplay2_s.chargenRom);

    return ret;
}


bool IniConfig::readConsole (ini_fd_t ini)
{
    bool ret = true;
    (void) ini_locateHeading (ini, "Console");
    ret &= readBool (ini, "Ansi",                console_s.ansi);
    ret &= readChar (ini, "Char Top Left",       console_s.topLeft);
    ret &= readChar (ini, "Char Top Right",      console_s.topRight);
    ret &= readChar (ini, "Char Bottom Left",    console_s.bottomLeft);
    ret &= readChar (ini, "Char Bottom Right",   console_s.bottomRight);
    ret &= readChar (ini, "Char Vertical",       console_s.vertical);
    ret &= readChar (ini, "Char Horizontal",     console_s.horizontal);
    ret &= readChar (ini, "Char Junction Left",  console_s.junctionLeft);
    ret &= readChar (ini, "Char Junction Right", console_s.junctionRight);
    return ret;
}


bool IniConfig::readAudio (ini_fd_t ini)
{
    bool ret = true;
    (void) ini_locateHeading (ini, "Audio");

    {
        int frequency = (int) audio_s.frequency;
        ret &= readInt (ini, "Frequency", frequency);
        audio_s.frequency = (unsigned long) frequency;
    }

    {
        int channels = 0;
        ret &= readInt (ini, "Channels",  channels);
        if (channels)
        {
            audio_s.playback = (channels == 1) ? SidConfig::MONO : SidConfig::STEREO;
        }
    }

    ret &= readInt (ini, "BitsPerSample", audio_s.precision);
    return ret;
}


bool IniConfig::readEmulation (ini_fd_t ini)
{
    bool ret = true;
    (void) ini_locateHeading (ini, "Emulation");

    ret &= readString (ini, "Engine", emulation_s.engine);

    {
        char *str;
        const bool res = readString (ini, "C64Model", str);
        if (res)
        {
            if (strcmp(str, "PAL") == 0)
                emulation_s.modelDefault = SidConfig::PAL;
            else if (strcmp(str, "NTSC") == 0)
                emulation_s.modelDefault = SidConfig::NTSC;
            else if (strcmp(str, "OLD_NTSC") == 0)
                emulation_s.modelDefault = SidConfig::OLD_NTSC;
            else if (strcmp(str, "DREAN") == 0)
                emulation_s.modelDefault = SidConfig::DREAN;
        }
        ret &= res;
    }

    ret &= readBool (ini, "ForceC64Model", emulation_s.modelForced);

    {
        char *str;
        const bool res = readString (ini, "SidModel", str);
        if (res)
        {
            if (strcmp(str, "MOS6581") == 0)
                emulation_s.sidModel = SidConfig::MOS6581;
            else if (strcmp(str, "MOS8580") == 0)
                emulation_s.sidModel = SidConfig::MOS8580;
        }
        ret &= res;
    }

    ret &= readBool (ini, "ForceSidModel", emulation_s.forceModel);

    ret &= readBool (ini, "UseFilter", emulation_s.filter);

    ret &= readDouble (ini, "FilterBias", emulation_s.bias);
    ret &= readDouble (ini, "FilterCurve6581", emulation_s.filterCurve6581);
    ret &= readInt (ini, "FilterCurve8580", emulation_s.filterCurve8580);

    return ret;
}

void IniConfig::read ()
{
    ini_fd_t ini = 0;

    std::string configPath;

    try
    {
        configPath = utils::getConfigPath();
    }
    catch (utils::error const &e)
    {
        goto IniConfig_read_error;
    }

    configPath.append("/").append(DIR_NAME);

#ifndef _WIN32
    // Make sure the config path exists
    if (!opendir(configPath.c_str()))
        mkdir(configPath.c_str(), 0755);
#else
    CreateDirectoryA(configPath.c_str(), NULL);
#endif

    configPath.append("/").append(FILE_NAME);

    // Opens an existing file or creates a new one
    ini = ini_open (configPath.c_str(), "w", ";");

    // Unable to open file?
    if (!ini)
        goto IniConfig_read_error;

    clear ();

    // This may not exist here...
    status &= readSidplay2  (ini);
    status &= readConsole   (ini);
    status &= readAudio     (ini);
    status &= readEmulation (ini);
    ini_close (ini);

    return;

IniConfig_read_error:
    if (ini)
        ini_close (ini);
    clear ();
    status = false;
}
