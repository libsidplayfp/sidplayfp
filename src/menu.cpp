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

#include "player.h"

#include "codeConvert.h"
#include "fmt/format.h"
#if defined(_WIN32) && defined(UNICODE)
#  include "fmt/xchar.h"
#endif

#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>

using std::string;

#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>

struct fill {
  char value;
  int width;
};

template <>
struct fmt::formatter<fill> {
  constexpr const char* parse(format_parse_context& ctx) const { return ctx.begin(); }

  fmt::basic_appender<char> format(fill f, format_context& ctx) const {
    return std::fill_n(ctx.out(), f.width, f.value);
  }
};

#ifdef FEAT_REGS_DUMP_SID
const char *noteName[] =
{
    "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
};
#endif

constexpr unsigned int tableWidth = 54;

const char SID6581[] = "MOS6581";
const char SID8580[] = "CSG8580";

const char* getModel(SidTuneInfo::model_t model)
{
    switch (model)
    {
    default:
    case SidTuneInfo::SIDMODEL_UNKNOWN:
        return "UNKNOWN";
    case SidTuneInfo::SIDMODEL_6581:
        return SID6581;
    case SidTuneInfo::SIDMODEL_8580:
        return SID8580;
    case SidTuneInfo::SIDMODEL_ANY:
        return "ANY";
    }
}

const char* getModel(SidConfig::sid_model_t model)
{
    switch (model)
    {
    default:
    case SidConfig::MOS6581:
        return SID6581;
    case SidConfig::MOS8580:
        return SID8580;
    }
}

const char* getClock(SidTuneInfo::clock_t clock)
{
    switch (clock)
    {
    default:
    case SidTuneInfo::CLOCK_UNKNOWN:
        return "UNKNOWN";
    case SidTuneInfo::CLOCK_PAL:
        return "PAL";
    case SidTuneInfo::CLOCK_NTSC:
        return "NTSC";
    case SidTuneInfo::CLOCK_ANY:
        return "ANY";
    }
}

string trimString(const char* str, unsigned int maxLen)
{
    string data(str);
    // avoid too long file names
    if (data.length() > maxLen)
    {
        data.resize(maxLen - 3);
        data.append("...");
    }
    return data;
}

#ifdef FEAT_REGS_DUMP_SID
const char *ConsolePlayer::getNote(uint16_t freq)
{
    if (freq)
    {
        int distance = 0xffff;
        for (int i=0; i<(12 * 8); i++)
        {
            int d = std::abs(freq - m_freqTable[i]);
            if (d < distance)
                distance = d;
            else
                return noteName[i-1];
        }
        return noteName[(12 * 8)-1];
    }

    return "---";
}
#endif

void ConsolePlayer::displayVersion()
{
    const SidInfo &info = m_engine.info();

    fmt::print("{} {}\n", PACKAGE_NAME, VERSION);
    fmt::print("Using {} {}\n", info.name(), info.version());
    fmt::print("Home Page: {}\n", PACKAGE_URL);
}

// Display console menu
void ConsolePlayer::menu ()
{
    if (m_quietLevel > 1)
        return;

    const SidInfo &info         = m_engine.info ();
    const SidTuneInfo *tuneInfo = m_tune.getInfo();

    if (m_verboseLevel > 1)
    {
        fmt::print("Config loaded from\n");
#if defined(_WIN32) && defined(UNICODE)
        fmt::print(L"{}\n", m_iniCfg.getFilename());
#else
        fmt::print("{}\n", m_iniCfg.getFilename());
#endif
    }

    // fmt::print("\n\f"); // New Page
    if ((m_iniCfg.console ()).ansi)
    {
        fmt::print("\x1b[40m");  // Background black
        fmt::print("\x1b[2J");   // Clear screen
        fmt::print("\x1b[0;0H"); // Move cursor to 0,0
        fmt::print("\x1b[?25l"); // and hide it
    }

    consoleTable (table_t::start);
    consoleTable (table_t::middle);
    consoleColour ((m_iniCfg.console()).title);
    fmt::print("  SIDPLAYFP - Music Player and C64 SID Chip Emulator\n");
    consoleTable  (table_t::middle);
    consoleColour ((m_iniCfg.console()).title);
    {
        string version;
        fmt::format_to(std::back_inserter(version), "sidplayfp {}, {} {}", VERSION, info.name(), info.version());
        fmt::print("{:^{}}\n", version, tableWidth);
    }

    color_t label_color = (m_iniCfg.console()).label_core;
    color_t text_color = (m_iniCfg.console()).text_core;

    const unsigned int n = tuneInfo->numberOfInfoStrings();
    if (n)
    {
        codeConvert codeset;

        consoleTable (table_t::separator);

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Title        : ");
        consoleColour (text_color);
        fmt::print("{}\n", codeset.convert(tuneInfo->infoString(0)));
        if (n>1)
        {
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print(" Author       : ");
            consoleColour (text_color);
            fmt::print("{}\n", codeset.convert(tuneInfo->infoString(1)));
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print(" Released     : ");
            consoleColour (text_color);
            fmt::print("{}\n", codeset.convert(tuneInfo->infoString(2)));
        }
    }

    for (unsigned int i = 0; i < tuneInfo->numberOfCommentStrings(); i++)
    {
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Comment      : ");
        consoleColour (text_color);
        fmt::print("{}\n", tuneInfo->commentString(i));
    }

    consoleTable (table_t::separator);

    label_color = (m_iniCfg.console()).label_extra;
    text_color = (m_iniCfg.console()).text_extra;

    if (m_verboseLevel)
    {
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" File format  : ");
        consoleColour (text_color);
        fmt::print("{}\n", tuneInfo->formatString());
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Filename(s)  : ");
        consoleColour (text_color);
        fmt::print("{}\n", trimString(tuneInfo->dataFileName(), 37));
        // Second file is only sometimes present
        if (tuneInfo->infoFileName())
        {
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print("              : ");
            consoleColour (text_color);
            fmt::print("{}\n", tuneInfo->infoFileName());
        }
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Condition    : ");
        consoleColour (text_color);
        fmt::print("{}\n", m_tune.statusString());

#if HAVE_TSID == 1
        if (!m_tsid)
        {
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print(" TSID Error   : ");
            consoleColour (text_color);
            fmt::print("{}\n", m_tsid.getError());
        }
#endif // HAVE_TSID
    }

    consoleTable  (table_t::middle);
    consoleColour (label_color);
    fmt::print(" Playlist     : ");
    consoleColour (text_color);

    {   // This will be the format used for playlists
        int i = 1;
        if (!m_track.single)
        {
            i  = m_track.selected;
            i -= (m_track.first - 1);
            if (i < 1)
                i += m_track.songs;
        }
        fmt::print("{}/{}", i, m_track.songs);
        fmt::print(" (tune ", tuneInfo->currentSong());
        fmt::print(" (tune {}/{} [{}])",
            tuneInfo->currentSong(),
            tuneInfo->songs(),
            tuneInfo->startSong());
    }

    if (m_track.loop)
        fmt::print(" [LOOPING]");
    fmt::print("\n");

    if (m_verboseLevel)
    {
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Song Speed   : ");
        consoleColour (text_color);
        fmt::print("{}\n", getClock(tuneInfo->clockSpeed()));
    }

    consoleTable  (table_t::middle);
    consoleColour (label_color);
    fmt::print(" Song Length  : ");
    consoleColour (text_color);
    if (m_timer.stop)
    {
        const uint_least32_t seconds = m_timer.stop / 1000;
        fmt::print("{:0>2}:{:0>2}.{:0>3}",
            ((seconds / 60) % 100),
            (seconds % 60),
            m_timer.stop % 1000);
    }
    else if (m_timer.valid)
        fmt::print("FOREVER");
    else if (songlengthDB == sldb_t::NONE)
        fmt::print("NO SLDB");
    else
        fmt::print("UNKNOWN");
    if (m_timer.start)
    {   // Show offset
        const uint_least32_t seconds = m_timer.start / 1000;
        fmt::print(" (+{:0>2}:{:0>2})",
            ((seconds / 60) % 100),
            (seconds % 60));
    }
    fmt::print("\n");

    if (m_verboseLevel)
    {
        consoleTable  (table_t::separator);

        label_color = (m_iniCfg.console()).label_core;
        text_color = (m_iniCfg.console()).text_core;

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Addresses    : ");
        consoleColour (text_color);
        // Display PSID Driver location
        fmt::print("DRIVER = ");
        if (info.driverAddr() == 0)
            fmt::print("NOT PRESENT");
        else
        {
            fmt::print("${:0>4X}-${:0>4X}",
                info.driverAddr(),
                info.driverAddr() + (info.driverLength() - 1));
        }
        fmt::print(", {} = ${:0>4X}\n",
            (tuneInfo->playAddr() == 0xffff) ? "SYS" : "INIT",
            tuneInfo->initAddr());
        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print("              : ");
        consoleColour (text_color);
        fmt::print("LOAD   = ${:0>4X}-${:0>4X}",
            tuneInfo->loadAddr(),
            tuneInfo->loadAddr() + (tuneInfo->c64dataLen() - 1));
        if (tuneInfo->playAddr() != 0xffff)
            fmt::print(", PLAY = ${:0>4X}",tuneInfo->playAddr());
        fmt::print("\n");

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" SID Details  : ");
        consoleColour (text_color);
        fmt::print("1st SID = $D400, Model = {}\n", getModel(tuneInfo->sidModel(0)));
        if (tuneInfo->sidChips() > 1)
        {
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print("              : ");
            consoleColour (text_color);
            fmt::print("2nd SID = ${:0>4X}", tuneInfo->sidChipBase(1));
            fmt::print(", Model = {}\n", getModel(tuneInfo->sidModel(1)));
            if (tuneInfo->sidChips() > 2)
            {
                consoleTable  (table_t::middle);
                consoleColour (label_color);
                fmt::print("              : ");
                consoleColour (text_color);
                fmt::print("3rd SID = ${:0>4X}", tuneInfo->sidChipBase(2));
                fmt::print(", Model = {}\n", getModel(tuneInfo->sidModel(2)));
            }
        }

        consoleTable  (table_t::separator);

        label_color = (m_iniCfg.console()).label_extra;
        text_color = (m_iniCfg.console()).text_extra;

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Play speed   : ");
        consoleColour (text_color);
        fmt::print("{}\n", info.speedString());

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" Play mode    : ");
        consoleColour (text_color);
        fmt::print("{}\n", (info.channels() == 1 ? "Mono" : "Stereo"));

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" SID Filter   : ");
        consoleColour (text_color);
        fmt::print("{}\n", (m_filter.enabled ? "Yes" : "No"));

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" DigiBoost    : ");
        consoleColour (text_color);
        fmt::print("{}\n", (m_engCfg.digiBoost ? "Yes" : "No"));

        consoleTable  (table_t::middle);
        consoleColour (label_color);
        fmt::print(" SID Model(s) : ");
        consoleColour (text_color);
#ifdef FEAT_SID_MODEL
        for (unsigned int i=0; i<info.numberOfSIDs(); i++)
            fmt::print("{} ", getModel(info.sidModel(i)));
        fmt::print("\n");
#else
        if (m_engCfg.forceSidModel)
            fmt::print("Forced ");
        else
            fmt::print("from tune, default = {}\n", getModel(m_engCfg.defaultSidModel));
#endif

        if (m_verboseLevel > 1)
        {
            consoleTable  (table_t::middle);
            consoleColour (label_color);
            fmt::print(" Delay        : ");
            consoleColour (text_color);
            fmt::print("{} (cycles at poweron)\n", info.powerOnDelay());
        }
    }

    const char* romDesc = info.kernalDesc();

    consoleTable  (table_t::separator);

    label_color = (m_iniCfg.console()).label_core;
    text_color = (m_iniCfg.console()).text_core;

    consoleTable  (table_t::middle);
    consoleColour (label_color);
    fmt::print(" Kernal ROM   : ");
    if (std::strlen(romDesc) == 0)
    {
        consoleColour (color_t::red);
        fmt::print("None - Some tunes may not play!\n");
    }
    else
    {
        consoleColour (text_color);
        fmt::print("{}\n", romDesc);
    }

    romDesc = info.basicDesc();

    consoleTable  (table_t::middle);
    consoleColour (label_color);
    fmt::print(" BASIC ROM    : ");
    if (std::strlen(romDesc) == 0)
    {
        consoleColour (color_t::red);
        fmt::print("None - Basic tunes will not play!\n");
    }
    else
    {
        consoleColour (text_color);
        fmt::print("{}\n", romDesc);
    }

    romDesc = info.chargenDesc();

    consoleTable  (table_t::middle);
    consoleColour (label_color);
    fmt::print(" Chargen ROM  : ");
    if (std::strlen(romDesc) == 0)
    {
        consoleColour (color_t::red);
        fmt::print("None\n");
    }
    else
    {
        consoleColour (text_color);
        fmt::print("{}\n", romDesc);
    }

    if (m_showhelp)
    {
        consoleTable(table_t::separator);
        consoleTable(table_t::middle);
        consoleColour((m_iniCfg.console()).title);
        fmt::print(" ←/→  Previous/Next  1-9  Toggle voices    p  Pause\n");
        consoleTable(table_t::middle);
        consoleColour((m_iniCfg.console()).title);
        fmt::print(" ↓/↑  Play speed     asd  Toggle samples   h  Help\n");
        consoleTable(table_t::middle);
        consoleColour((m_iniCfg.console()).title);
        fmt::print(" ⇱/⇲  First/Last     f    Toggle filter    q  Quit\n");
    }

#ifdef FEAT_REGS_DUMP_SID
    if (m_verboseLevel > 1)
    {
        consoleTable (table_t::separator);
        consoleTable (table_t::middle);
        fmt::print("         NOTE PW         CONTROL          WAVEFORMS\n");

#ifdef FEAT_NEW_PLAY_API
        for (unsigned int i=0; i < m_engine.installedSIDs() * 3; i++)
#else
        for (int i=0; i < tuneInfo->sidChips() * 3; i++)
#endif
        {
            consoleTable (table_t::middle);
            fmt::print("\n"); // reserve space for each voice's status
        }
    }
#endif

    consoleTable (table_t::end);

    if (m_driver.file)
        fmt::print("Creating audio file, please wait...");
    else
        fmt::print("Playing, press ESC to stop...");

    // Get all the text to the screen so music playback
    // is not disturbed.
    if ( !m_quietLevel )
        fmt::print("00:00");

    consoleRestore();

    std::fflush(stdout);
}

void ConsolePlayer::refreshRegDump()
{
#ifdef FEAT_REGS_DUMP_SID
    if (m_verboseLevel > 1)
    {
        unsigned int chips =
#ifdef FEAT_NEW_PLAY_API
            m_engine.installedSIDs();
#else
            m_tune.getInfo()->sidChips();
#endif
        fmt::print("\x1b[{}A\r", chips * 3 + 1); // Moves cursor X lines up

        const color_t ctrlon  = (m_iniCfg.console()).control_on;
        const color_t ctrloff = (m_iniCfg.console()).control_off;
        for (unsigned int j=0; j < chips; j++)
        {
            uint8_t* registers = m_registers[j];
            uint8_t oldCtl[3];
            oldCtl[0] = registers[0x04];
            oldCtl[1] = registers[0x0b];
            oldCtl[2] = registers[0x12];

            if (m_engine.getSidStatus(j, registers))
            {
                oldCtl[0] ^= registers[0x04];
                oldCtl[1] ^= registers[0x0b];
                oldCtl[2] ^= registers[0x12];

                for (int i=0; i < 3; i++)
                {
                    consoleTable(table_t::middle);
                    consoleColour(ctrloff);

                    fmt::print(" Voice {}", (j * 3 + i+1));

                    consoleColour((m_iniCfg.console()).notes);

                    fmt::print(" {} ${:0>3X}",
                        getNote(registers[0x00 + i * 0x07] | (registers[0x01 + i * 0x07] << 8)),
                        registers[0x02 + i * 0x07] | ((registers[0x03 + i * 0x07] & 0x0f) << 8));

                    // gate changed ?
                    consoleColour((oldCtl[i] & 0x01) ? ctrlon : ctrloff);
                    // gate on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x01) ? " GATE" : " gate");

                    // sync changed ?
                    consoleColour((oldCtl[i] & 0x02) ? ctrlon : ctrloff);
                    // sync on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x02) ? " SYNC" : " sync");

                    // ring changed ?
                    consoleColour((oldCtl[i] & 0x04) ? ctrlon : ctrloff);
                    // ring on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x04) ? " RING" : " ring");

                    // test changed ?
                    consoleColour((oldCtl[i] & 0x08) ? ctrlon : ctrloff);
                    // test on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x08) ? " TEST" : " test");

                    // triangle changed ?
                    consoleColour((oldCtl[i] & 0x10) ? ctrlon : ctrloff);
                    // triangle on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x10) ? " TRI" : " ___");

                    // sawtooth changed ?
                    consoleColour((oldCtl[i] & 0x20) ? ctrlon : ctrloff);
                    // sawtooth on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x20) ? " SAW" : " ___");

                    // pulse changed ?
                    consoleColour((oldCtl[i] & 0x40) ? ctrlon : ctrloff);
                    // pulse on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x40) ? " PUL" : " ___");

                    // noise changed ?
                    consoleColour((oldCtl[i] & 0x80) ? ctrlon : ctrloff);
                    // noise on ?
                    fmt::print("{}", (registers[0x04 + i * 0x07] & 0x80) ? " NOI" : " ___");

                    fmt::print("\n");
                }
            }
            else
            {
                consoleTable(table_t::middle); fmt::print("???\n");
                consoleTable(table_t::middle); fmt::print("???\n");
                consoleTable(table_t::middle); fmt::print("???\n");
            }
        }

        consoleTable(table_t::end);
    }
    else
#endif
        fmt::print("\r");

    if (m_driver.file)
        fmt::print("Creating audio file, please wait...");
    else
        fmt::print("Playing, press ESC to stop...");

    std::fflush(stdout);
}

// Set colour of text on console
void ConsolePlayer::consoleColour(color_t colour)
{
    if ((m_iniCfg.console ()).ansi)
    {
        const char *fg = "";

        switch (colour)
        {
        case color_t::black:          fg = "0;30"; break;
        case color_t::red:            fg = "0;31"; break;
        case color_t::green:          fg = "0;32"; break;
        case color_t::yellow:         fg = "0;33"; break;
        case color_t::blue:           fg = "0;34"; break;
        case color_t::magenta:        fg = "0;35"; break;
        case color_t::cyan:           fg = "0;36"; break;
        case color_t::white:          fg = "0;37"; break;
        case color_t::bright_black:   fg = "1;30"; break;
        case color_t::bright_red:     fg = "1;31"; break;
        case color_t::bright_green:   fg = "1;32"; break;
        case color_t::bright_yellow:  fg = "1;33"; break;
        case color_t::bright_blue:    fg = "1;34"; break;
        case color_t::bright_magenta: fg = "1;35"; break;
        case color_t::bright_cyan:    fg = "1;36"; break;
        case color_t::bright_white:   fg = "1;37"; break;
        }

        // Add black background
        const char *bg = ";40";

        fmt::print("\x1b[{}{}m", fg, bg);
    }
}

// Display menu outline
void ConsolePlayer::consoleTable(table_t table)
{
    consoleColour((m_iniCfg.console()).decorations);
    switch (table)
    {
        case table_t::start:
        fmt::print("{}{}{}",
                   (m_iniCfg.console()).topLeft,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).topRight);
        break;

    case table_t::middle:
        fmt::print("{0}{1}\r{1}",
                   fill{' ', tableWidth+1},
                   (m_iniCfg.console()).vertical);
        return;

    case table_t::separator:
        fmt::print("{}{}{}",
                   (m_iniCfg.console()).junctionRight,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).junctionLeft);
        break;

    case table_t::end:
        fmt::print("{}{}{}",
                   (m_iniCfg.console()).bottomLeft,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).bottomRight);
        break;
    }

    // Move back to begining of row and skip first char
    fmt::print("\n");
}


// Restore Ansi Console to defaults
void ConsolePlayer::consoleRestore ()
{
    if ((m_iniCfg.console ()).ansi) {
        fmt::print("\x1b[?25h");
        fmt::print("\x1b[0m");
    }
}
