/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2011-2026 Leandro Nini
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

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/stat.h>  /* mkdir */
#  include <dirent.h>    /* opendir */
#else
#  include <windows.h>
#endif

#include "utils.h"
#include "dataParser.h"

#include "sidcxx11.h"

#undef SEPARATOR
#define SEPARATOR "/"

const char* colorStrings[16] =
{
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "magenta",
    "cyan",
    "white",
    "bright black",
    "bright red",
    "bright green",
    "bright yellow",
    "bright blue",
    "bright magenta",
    "bright cyan",
    "bright white"
};

inline void debug(MAYBE_UNUSED const char *msg, MAYBE_UNUSED const char *val)
{
#ifndef NDEBUG
    fmt::print("{}{}\n", msg, val);
#endif
}

inline void error(const char *msg)
{
    fmt::print(stderr, "{}\n", msg);
}

inline void error(const char *msg, const char *val)
{
    fmt::print(stderr, "{}{}\n", msg, val);
}

const char *DIR_NAME = "sidplayfp";
const char *FILE_NAME = "sidplayfp.ini";


IniConfig::IniConfig()
{   // Initialise everything else
    clear();
}


IniConfig::~IniConfig()
{
    clear();
}


void IniConfig::clear()
{
    sidplay2_s.version      = 1;           // INI File Version
    sidplay2_s.database.clear();
    sidplay2_s.playLength   = 0;           // INFINITE
    sidplay2_s.recordLength = (3 * 60 + 30) * 1000; // 3.5 minutes
    sidplay2_s.kernalRom.clear();
    sidplay2_s.basicRom.clear();
    sidplay2_s.chargenRom.clear();
    sidplay2_s.verboseLevel = 0;

    console_s.ansi          = false;
    console_s.topLeft       = "┌";
    console_s.topRight      = "┐";
    console_s.bottomLeft    = "└";
    console_s.bottomRight   = "┘";
    console_s.vertical      = "│";
    console_s.horizontal    = "─";
    console_s.junctionLeft  = "┤";
    console_s.junctionRight = "├";
    console_s.decorations   = color_t::bright_white;
    console_s.title         = color_t::white;
    console_s.label_core    = color_t::bright_green;
    console_s.text_core     = color_t::bright_yellow;
    console_s.label_extra   = color_t::bright_magenta;
    console_s.text_extra    = color_t::bright_cyan;
    console_s.notes         = color_t::bright_blue;
    console_s.control_on    = color_t::bright_green;
    console_s.control_off   = color_t::bright_red;

    audio_s.frequency = SidConfig::DEFAULT_SAMPLING_FREQ;
    audio_s.channels  = 0;
    audio_s.precision = 16;
    audio_s.bufLength = 250;

    emulation_s.modelDefault  = SidConfig::PAL;
    emulation_s.modelForced   = false;
    emulation_s.sidModel      = SidConfig::MOS6581;
    emulation_s.forceModel    = false;
    emulation_s.ciaModel      = SidConfig::MOS6526;
    emulation_s.filter        = true;
    emulation_s.engine.clear();

    emulation_s.bias            = 0.5;
    emulation_s.filterCurve6581 = 0.5;
#ifdef FEAT_FILTER_RANGE
    emulation_s.filterRange6581 = 0.5;
#endif
    emulation_s.filterCurve8580 = 0.5;
#ifdef FEAT_CW_STRENGTH
    emulation_s.combinedWaveformsStrength = SidConfig::AVERAGE;
#endif
    emulation_s.powerOnDelay = -1;
    emulation_s.samplingMethod = SidConfig::RESAMPLE_INTERPOLATE;
    emulation_s.fastSampling = false;
}


// static helpers

const char* readKey(iniHandler &ini, const char *key)
{
    const char* value = ini.getValue(key);
    if (value == nullptr)
    {   // Doesn't exist, add it
        ini.addValue(key, "");
        debug("Key doesn't exist: ", key);
    }
    else if (!value[0])
    {
        // Ignore empty values
        return nullptr;
    }

    return value;
}

void readDouble(iniHandler &ini, const char *key, double &result)
{
    const char* value = readKey(ini, key);
    if (value == nullptr)
        return;

    try
    {
        result = dataParser::parseDouble(value);
    }
    catch (dataParser::parseError const &e)
    {
        error("Error parsing double at ", key);
    }
}


void readInt(iniHandler &ini, const char *key, int &result)
{
    const char* value = readKey(ini, key);
    if (value == nullptr)
        return;

    try
    {
        result = dataParser::parseInt(value);
    }
    catch (dataParser::parseError const &e)
    {
        error("Error parsing int at ", key);
    }
}


void readBool(iniHandler &ini, const char *key, bool &result)
{
    const char* value = readKey(ini, key);
    if (value == nullptr)
        return;

    try
    {
        result = dataParser::parseBool(value);
    }
    catch (dataParser::parseError const &e)
    {
        error("Error parsing bool at ", key);
    }
}


std::string readString(iniHandler &ini, const char *key)
{
    const char* value = ini.getValue(key);
    if (value == nullptr)
    {
        // Doesn't exist, add it
        ini.addValue(key, "");
        debug("Key doesn't exist: ", key);
        return std::string();
    }

    return std::string(value);
}


void readChar(iniHandler &ini, const char *key, char &ch)
{
    std::string str = readString(ini, key);
    if (str.empty())
        return;

    char c = 0;

    // Check if we have an actual character
    if (str[0] == '\'')
    {
        if (str[2] != '\'')
            return;
        else
            c = str[1];
    } // Nope is number
    else
    {
        try
        {
            c = dataParser::parseInt(str.c_str());
        }
        catch (dataParser::parseError const &e)
        {
            error("Error parsing int at ", key);
        }
    }

    // Clip off special characters
    if ((unsigned) c >= 32)
        ch = c;
}


bool readTime(iniHandler &ini, const char *key, int &value)
{
    std::string str = readString(ini, key);
    if (str.empty())
        return false;

    int time;
    int milliseconds = 0;
    const size_t sep = str.find_first_of(':');
    const size_t dot = str.find_first_of('.');
    try
    {
        if (sep == std::string::npos)
        {   // User gave seconds
            time = dataParser::parseInt(str.c_str());
        }
        else
        {   // Read in MM:SS.mmm format
            const int min = dataParser::parseInt(str.substr(0, sep).c_str());
            if (min < 0 || min > 99)
                goto IniCofig_readTime_error;
            time = min * 60;

            int sec;
            if (dot == std::string::npos)
            {
                sec  = dataParser::parseInt(str.substr(sep + 1).c_str());
            }
            else
            {
                sec  = dataParser::parseInt(str.substr(sep + 1, dot - sep).c_str());
                std::string msec = str.substr(dot + 1);
                milliseconds = dataParser::parseInt(msec.c_str());
                switch (msec.length())
                {
                case 1: milliseconds *= 100; break;
                case 2: milliseconds *= 10; break;
                case 3: break;
                default: goto IniCofig_readTime_error;
                }
            }

            if (sec < 0 || sec > 59)
                goto IniCofig_readTime_error;
            time += sec;
        }
    }
    catch (dataParser::parseError const &e)
    {
        error("Error parsing time at ", key);
        return false;
    }

    value = time * 1000 + milliseconds;
    return true;

IniCofig_readTime_error:
    error("Invalid time at ", key);
    return false;
}

bool readColor(iniHandler &ini, const char *key, color_t &ch)
{
    std::string str = readString(ini, key);
    if (str.empty())
        return false;

    for (int i=0; i<16; i++)
    {
        if (str.compare(colorStrings[i]) == 0)
        {
            ch = static_cast<color_t>(i);
            return true;
        }
    }

    return false;
}


void IniConfig::readSidplay2(iniHandler &ini)
{
    if (!ini.setSection("SIDPlayfp"))
        ini.addSection("SIDPlayfp");

    int version = sidplay2_s.version;
    readInt(ini, "Version", version);
    if (version > 0)
        sidplay2_s.version = version;

    sidplay2_s.database = readString(ini, "Songlength Database");

    if (sidplay2_s.database.empty())
    {
        std::string buffer(utils::getDataPath());
        buffer.append(SEPARATOR).append(DIR_NAME).append(SEPARATOR).append("Songlengths.txt");
        fs::path f(buffer);
        std::error_code ec; // For noexcept overload usage.
        if (fs::is_regular_file(f) && fs::exists(f, ec) && !ec)
        {
            auto perms = fs::status(f, ec).permissions();
            if ((perms & fs::perms::owner_read) != fs::perms::none &&
                (perms & fs::perms::group_read) != fs::perms::none &&
                (perms & fs::perms::others_read) != fs::perms::none
            )
                sidplay2_s.database.assign(buffer);
        }
    }

    int time;
    if (readTime(ini, "Default Play Length", time))
        sidplay2_s.playLength = time;
    if (readTime(ini, "Default Record Length", time))
        sidplay2_s.recordLength = time;

    sidplay2_s.kernalRom = readString(ini, "Kernal Rom");
    sidplay2_s.basicRom = readString(ini, "Basic Rom");
    sidplay2_s.chargenRom = readString(ini, "Chargen Rom");

    readInt(ini, "VerboseLevel", sidplay2_s.verboseLevel);
}


void IniConfig::readConsole(iniHandler &ini)
{
    if (!ini.setSection ("Console"))
        ini.addSection("Console");

    bool ascii;
    readBool(ini, "ASCII", ascii);
    if (ascii)
    {
        console_s.topLeft       = "+";
        console_s.topRight      = "+";
        console_s.bottomLeft    = "+";
        console_s.bottomRight   = "+";
        console_s.vertical      = "|";
        console_s.horizontal    = "-";
        console_s.junctionLeft  = "+";
        console_s.junctionRight = "+";
    }

    readBool(ini, "Ansi",                console_s.ansi);
//     readChar(ini, "Char Top Left",       console_s.topLeft);
//     readChar(ini, "Char Top Right",      console_s.topRight);
//     readChar(ini, "Char Bottom Left",    console_s.bottomLeft);
//     readChar(ini, "Char Bottom Right",   console_s.bottomRight);
//     readChar(ini, "Char Vertical",       console_s.vertical);
//     readChar(ini, "Char Horizontal",     console_s.horizontal);
//     readChar(ini, "Char Junction Left",  console_s.junctionLeft);
//     readChar(ini, "Char Junction Right", console_s.junctionRight);

    readColor(ini, "Color Decorations",  console_s.decorations);
    readColor(ini, "Color Title",        console_s.title);
    readColor(ini, "Color Label Core",   console_s.label_core);
    readColor(ini, "Color Text Core",    console_s.text_core);
    readColor(ini, "Color Label Extra",  console_s.label_extra);
    readColor(ini, "Color Text Extra",   console_s.text_extra);
    readColor(ini, "Color Notes",        console_s.notes);
    readColor(ini, "Color Control On",   console_s.control_on);
    readColor(ini, "Color Control Off",  console_s.control_off);
}


void IniConfig::readAudio(iniHandler &ini)
{
    if (!ini.setSection ("Audio"))
        ini.addSection("Audio");

    readInt(ini, "Frequency", audio_s.frequency);

    readInt(ini, "Channels",  audio_s.channels);

    readInt(ini, "BitsPerSample", audio_s.precision);

    readInt(ini, "BufferLength", audio_s.bufLength);
}


void IniConfig::readEmulation(iniHandler &ini)
{
    if (!ini.setSection ("Emulation"))
        ini.addSection("Emulation");

    emulation_s.engine = readString (ini, "Engine");

    {
        std::string str = readString (ini, "C64Model");
        if (!str.empty())
        {
            if (str.compare("PAL") == 0)
                emulation_s.modelDefault = SidConfig::PAL;
            else if (str.compare("NTSC") == 0)
                emulation_s.modelDefault = SidConfig::NTSC;
            else if (str.compare("OLD_NTSC") == 0)
                emulation_s.modelDefault = SidConfig::OLD_NTSC;
            else if (str.compare("DREAN") == 0)
                emulation_s.modelDefault = SidConfig::DREAN;
        }
    }

    readBool(ini, "ForceC64Model", emulation_s.modelForced);
    readBool(ini, "DigiBoost", emulation_s.digiboost);
    {
        std::string str = readString(ini, "CiaModel");
        if (!str.empty())
        {
            if (str.compare("MOS6526") == 0)
                emulation_s.ciaModel = SidConfig::MOS6526;
            else if (str.compare("MOS8521") == 0)
                emulation_s.ciaModel = SidConfig::MOS8521;
        }
    }
    {
        std::string str = readString(ini, "SidModel");
        if (!str.empty())
        {
            if (str.compare("MOS6581") == 0)
                emulation_s.sidModel = SidConfig::MOS6581;
            else if (str.compare("MOS8580") == 0)
                emulation_s.sidModel = SidConfig::MOS8580;
        }
    }

    readBool(ini, "ForceSidModel", emulation_s.forceModel);

    readBool(ini, "UseFilter", emulation_s.filter);

    readDouble(ini, "FilterBias", emulation_s.bias);
    readDouble(ini, "FilterCurve6581", emulation_s.filterCurve6581);
#ifdef FEAT_FILTER_RANGE
    {
        // For backward compatibility, remove for 3.0
        const char *key = "filterRange6581";
        const char* value = ini.getValue(key);
        if (value && value[0])
        {
            ini.addValue("FilterRange6581", value);
            ini.removeValue(key);
        }
    }
    readDouble(ini, "FilterRange6581", emulation_s.filterRange6581);

#endif
    readDouble(ini, "FilterCurve8580", emulation_s.filterCurve8580);

#ifdef FEAT_CW_STRENGTH
    {
        std::string str = readString(ini, "CombinedWaveforms");
        if (!str.empty())
        {
            if (str.compare("AVERAGE") == 0)
                emulation_s.combinedWaveformsStrength = SidConfig::AVERAGE;
            else if (str.compare("WEAK") == 0)
                emulation_s.combinedWaveformsStrength = SidConfig::WEAK;
            else if (str.compare("STRONG") == 0)
                emulation_s.combinedWaveformsStrength = SidConfig::STRONG;
        }
    }
#endif

    readInt(ini, "PowerOnDelay", emulation_s.powerOnDelay);

    {
        std::string str = readString(ini, "Sampling");
        if (!str.empty())
        {
            if (str.compare("INTERPOLATE") == 0)
                emulation_s.samplingMethod = SidConfig::INTERPOLATE;
            else if (str.compare("RESAMPLE") == 0)
                emulation_s.samplingMethod = SidConfig::RESAMPLE_INTERPOLATE;
        }
    }

    readBool(ini, "ResidFastSampling", emulation_s.fastSampling);
}

class iniError
{
private:
    const std::string msg;

public:
    iniError(const char* msg) : msg(msg) {}
    const std::string message() const { return msg; }
};

void createDir(const std::string& path)
{
#ifndef _WIN32
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
    }
    else if (errno == ENOENT)
    {
        if (mkdir(path.c_str(), 0755) < 0)
        {
            throw iniError(strerror(errno));
        }
    }
    else
    {
        throw iniError(strerror(errno));
    }
#else
    // TODO MB2WC
#ifdef UNICODE
    std::wstring p = utils::utf8_decode(path.c_str());
#else
    std::string p = path;
#endif
    if (GetFileAttributes(p.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        if (CreateDirectory(p.c_str(), NULL) == 0)
        {
            LPTSTR pBuffer;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuffer, 0, NULL);
#ifdef UNICODE
    std::string e = utils::utf8_encode(pBuffer);
#else
    std::string e = pBuffer;
#endif
            iniError err(e.c_str());
            LocalFree(pBuffer);
            throw err;
        }
    }
#endif
}

std::string getConfigPath()
{
    std::string configPath;

    try
    {
        configPath = utils::getConfigPath();
    }
    catch (utils::error const &e)
    {
        throw iniError("Cannot get config path!");
    }

    debug("Config path: ", configPath.c_str());

    // Make sure the config path exists
    createDir(configPath);

    configPath.append(SEPARATOR).append(DIR_NAME);

    // Make sure the app config path exists
    createDir(configPath);

    configPath.append(SEPARATOR).append(FILE_NAME);

    debug("Config file: ", configPath.c_str());

    return configPath;
}

bool tryOpen(MAYBE_UNUSED iniHandler &ini)
{
#ifdef _WIN32
    {
        // Try exec dir first
        std::string execPath(utils::getExecPath());
        execPath.append(SEPARATOR).append(FILE_NAME);
        if (ini.tryOpen(execPath.c_str()))
            return true;
    }
#endif
    return false;
}

void IniConfig::read()
{
    clear();

    iniHandler ini;

    if (!tryOpen(ini))
    {
        try
        {
            std::string configPath = getConfigPath();

            // Opens an existing file or creates a new one
            if (!ini.open(configPath.c_str()))
            {
                error("Error reading config file!");
                return;
            }
        } catch (iniError const &e) {
            error(e.message().c_str());
            return;
        }
    }

    readSidplay2  (ini);
    readConsole   (ini);
    readAudio     (ini);
    readEmulation (ini);

    m_fileName = ini.getFilename();

    ini.close();
}
