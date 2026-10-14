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

#include <fmt/format.h>
#if defined(_WIN32) && defined(UNICODE)
#  include <fmt/xchar.h>
#endif

#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdio>

#include <string>
#include <vector>
#include <algorithm>

using std::string;

#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>

struct fill {
  std::string value;
  int width;
};

template <>
struct fmt::formatter<fill> {
  static constexpr const char* parse(format_parse_context& ctx) { return ctx.begin(); }

  static fmt::basic_appender<char> format(fill f, format_context& ctx) {
    auto it = ctx.out();
    for (int i = 0; i < f.width; ++i)
        it = std::copy_n(f.value.begin(), f.value.size(), it);
    return it;
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

// Width of the value column inside the table (label is 16 columns)
constexpr unsigned int valueWidth = tableWidth - 16;

// Display width of a UTF-8 string in code points (continuation bytes not counted)
size_t utf8Width(const std::string& s)
{
    size_t cols = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80)
            cols++;
    }
    return cols;
}

// Split text into lines of at most width code points, breaking only at spaces.
// The break space is dropped. A word longer than width gets its own unbroken line.
// Text that fits is returned unchanged.
std::vector<std::string> wrapText(const std::string& s, const unsigned int width)
{
    std::vector<std::string> lines;
    if (utf8Width(s) <= width)
    {
        lines.push_back(s);
        return lines;
    }
    std::string line;
    size_t pos = 0;
    while (pos < s.size())
    {
        const size_t end = std::min(s.find(' ', pos), s.size());
        const std::string word = s.substr(pos, end - pos);
        pos = end + 1;
        if (word.empty())
            continue;
        if (line.empty())
            line = word;
        else if (utf8Width(line) + 1 + utf8Width(word) <= width)
            line += ' ' + word;
        else
        {
            lines.push_back(line);
            line = word;
        }
    }
    if (!line.empty() || lines.empty())
        lines.push_back(line);
    return lines;
}

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

const char* getEmu(SIDEMUS emu)
{
    switch (emu)
    {
    default:
    case EMU_RESIDFP:
        return "RESIDFP";
    case EMU_SIDLITE:
        return "SIDLITE";
    case EMU_RESID:
        return "RESID";
    case EMU_HARDSID:
        return "HARDSID";
    case EMU_EXSID:
        return "EXSID";
    case EMU_USBSID:
        return "USBSID";
    }
}

// Shorten to maxLen code points with a trailing "...", never splitting a UTF-8 sequence
string trimString(const string& data, unsigned int maxLen)
{
    if (utf8Width(data) <= maxLen)
        return data;
    // keep maxLen - 3 code points, then cut at the next lead byte
    size_t keep = maxLen - 3;
    size_t pos = 0;
    while (pos < data.size())
    {
        if ((static_cast<unsigned char>(data[pos]) & 0xC0) != 0x80)
        {
            if (keep == 0)
                break;
            keep--;
        }
        pos++;
    }
    return data.substr(0, pos) + "...";
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
void ConsolePlayer::menu()
{
    if (m_verboseLevel < -1)
        return;

    const SidInfo &info         = m_engine.info ();
    const SidTuneInfo *tuneInfo = m_tune.getInfo();

    fmt::print("\n\f"); // New Page
    if (m_iniCfg.console().ansi)
    {
        fmt::print("\x1b[40m");  // Background black
        fmt::print("\x1b[H");    // Move cursor to 0,0
        fmt::print("\x1b[J");    // Clear screen
        fmt::print("\x1b[?25l"); // and hide it

        m_console_inited = true;
    }

    consoleTable(table_t::start);
    consoleTable(table_t::middle);
    sid_print(fg((m_iniCfg.console()).title), "  SIDPLAYFP - Music Player and C64 SID Chip Emulator\n");
    consoleTable(table_t::middle);
    {
        string version;
        fmt::format_to(std::back_inserter(version), "sidplayfp {}, {} {}", VERSION, info.name(), info.version());
        sid_print(fg((m_iniCfg.console()).title), "{:^{}}\n", version, tableWidth);
    }

    color_t label_color = (m_iniCfg.console()).label_core;
    color_t text_color = (m_iniCfg.console()).text_core;

#ifdef FEAT_SID_MODEL
    // Print text wrapped to the table, continuation rows get a blank label
    auto printWrapped = [&](const std::string& text, const color_t lc, const color_t tc, const char* tail)
    {
        const std::vector<std::string> lines = wrapText(text, valueWidth);
        for (size_t l = 0; l < lines.size(); l++)
        {
            if (l)
            {
                sid_print(fg(tc), "\n"); // end previous row, like single rows do
                consoleTable(table_t::middle);
                sid_print(fg(lc), "              : ");
            }
            sid_print(fg(tc), "{}", lines[l]);
        }
        sid_print(fg(tc), "{}\n", tail);
    };
#endif

    const unsigned int n = tuneInfo->numberOfInfoStrings();
    if (n)
    {
        codeConvert codeset;

        consoleTable(table_t::separator);

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Title        : ");
        sid_print(fg(text_color), "{}\n", trimString(codeset.convert(tuneInfo->infoString(0)), valueWidth));
        if (n>1)
        {
            consoleTable(table_t::middle);
            sid_print(fg(label_color), " Author       : ");
            sid_print(fg(text_color), "{}\n", trimString(codeset.convert(tuneInfo->infoString(1)), valueWidth));
            consoleTable(table_t::middle);
            sid_print(fg(label_color), " Released     : ");
            sid_print(fg(text_color), "{}\n", trimString(codeset.convert(tuneInfo->infoString(2)), valueWidth));
        }
    }

    for (unsigned int i = 0; i < tuneInfo->numberOfCommentStrings(); i++)
    {
        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Comment      : ");
        sid_print(fg(text_color), "{}\n", tuneInfo->commentString(i));
    }

    consoleTable (table_t::separator);

    label_color = (m_iniCfg.console()).label_extra;
    text_color = (m_iniCfg.console()).text_extra;

    if (m_verboseLevel)
    {
        consoleTable(table_t::middle);
        sid_print(fg(label_color), " File format  : ");
        sid_print(fg(text_color), "{}\n", tuneInfo->formatString());
        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Filename(s)  : ");
        sid_print(fg(text_color), "{}\n", trimString(tuneInfo->dataFileName(), 37));
        // Second file is only sometimes present
        if (tuneInfo->infoFileName())
        {
            consoleTable(table_t::middle);
            sid_print(fg(label_color), "              : ");
            sid_print(fg(text_color), "{}\n", tuneInfo->infoFileName());
        }
        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Condition    : ");
        sid_print(fg(text_color), "{}\n", m_tune.statusString());

#if HAVE_TSID == 1
        if (!m_tsid)
        {
            consoleTable(table_t::middle);
            sid_print(fg(label_color), " TSID Error   : ");
            sid_print(fg(text_color), "{}\n", m_tsid.getError());
        }
#endif // HAVE_TSID
    }

    consoleTable(table_t::middle);
    sid_print(fg(label_color), " Playlist     : ");

    {   // This will be the format used for playlists
        int i = 1;
        if (!m_track.single)
        {
            i  = m_track.selected;
            i -= (m_track.first - 1);
            if (i < 1)
                i += m_track.songs;
        }
        sid_print(fg(text_color), "{}/{}", i, m_track.songs);
        sid_print(fg(text_color), " (tune {}/{} [{}])",
            tuneInfo->currentSong(),
            tuneInfo->songs(),
            tuneInfo->startSong());
    }

    if (m_track.loop)
        fmt::print(" [LOOPING]");
    fmt::print("\n");

    if (m_verboseLevel)
    {
        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Song Speed   : ");
        sid_print(fg(text_color), "{}\n", getClock(tuneInfo->clockSpeed()));
    }

    consoleTable(table_t::middle);
    sid_print(fg(label_color), " Song Length  : ");
    if (m_timer.stop)
    {
        const uint_least32_t seconds = m_timer.stop / 1000;
        sid_print(fg(text_color), "{:0>2}:{:0>2}.{:0>3}",
            ((seconds / 60) % 100),
            (seconds % 60),
            m_timer.stop % 1000);
    }
    else if (m_timer.valid)
        sid_print(fg(text_color), "FOREVER");
    else if (songlengthDB == sldb_t::NONE)
        sid_print(fg(text_color), "NO SLDB");
    else
        sid_print(fg(text_color), "UNKNOWN");
    if (m_timer.start)
    {   // Show offset
        const uint_least32_t seconds = m_timer.start / 1000;
        sid_print(fg(text_color), " (+{:0>2}:{:0>2})",
            ((seconds / 60) % 100),
            (seconds % 60));
    }
    fmt::print("\n");

    if (m_verboseLevel)
    {
        consoleTable(table_t::separator);

        label_color = (m_iniCfg.console()).label_core;
        text_color = (m_iniCfg.console()).text_core;

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Addresses    : ");
        // Display PSID Driver location
        sid_print(fg(text_color), "DRIVER = ");
        if (info.driverAddr() == 0)
            sid_print(fg(text_color), "NOT PRESENT");
        else
        {
            sid_print(fg(text_color), "${:0>4X}-${:0>4X}",
                info.driverAddr(),
                info.driverAddr() + (info.driverLength() - 1));
        }
        sid_print(fg(text_color), ", {} = ${:0>4X}\n",
            (tuneInfo->playAddr() == 0xffff) ? "SYS" : "INIT",
            tuneInfo->initAddr());
        consoleTable(table_t::middle);
        sid_print(fg(label_color), "              : ");
        sid_print(fg(text_color), "LOAD   = ${:0>4X}-${:0>4X}",
            tuneInfo->loadAddr(),
            tuneInfo->loadAddr() + (tuneInfo->c64dataLen() - 1));
        if (tuneInfo->playAddr() != 0xffff)
            sid_print(fg(text_color), ", PLAY = ${:0>4X}",tuneInfo->playAddr());
        fmt::print("\n");

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " SID Details  : ");
#ifdef FEAT_SID_V5
        const bool showPan = tuneInfo->hasPanning() && (tuneInfo->sidChips() > 1) && (m_driver.cfg.channels != 1);
#endif
        // Pan tag appended to each SID row, empty unless a stereo panning hint applies
        auto panTag = [&](unsigned int i) -> const char*
        {
#ifdef FEAT_SID_V5
            if (!showPan)
                return "";
            switch (tuneInfo->sidPanning(i))
            {
            case SidTuneInfo::PAN_LEFT:
                return " (L)";
            case SidTuneInfo::PAN_RIGHT:
                return " (R)";
            default:
                return " (C)";
            }
#else
            (void)i;
            return "";
#endif
        };
        sid_print(fg(text_color), "1st SID = $D400, Model = {}{}\n", getModel(tuneInfo->sidModel(0)), panTag(0));
#ifdef FEAT_SID_V5
        const bool listAllSids = (tuneInfo->sidChips() > 3);
#else
        const bool listAllSids = false;
#endif
        if (listAllSids)
        {
            // SID v5 tunes with more than 3 chips: list every chip
            for (unsigned int i=1; i<static_cast<unsigned int>(tuneInfo->sidChips()); i++)
            {
                consoleTable(table_t::middle);
                sid_print(fg(label_color), "              : ");
                sid_print(fg(text_color), "{}{} SID = ${:0>4X}", i + 1,
                    (i == 1) ? "nd" : (i == 2) ? "rd" : "th", tuneInfo->sidChipBase(i));
                sid_print(fg(text_color), ", Model = {}{}\n", getModel(tuneInfo->sidModel(i)), panTag(i));
            }
        }
        else if (tuneInfo->sidChips() > 1)
        {
            consoleTable(table_t::middle);
            sid_print(fg(label_color), "              : ");
            sid_print(fg(text_color), "2nd SID = ${:0>4X}", tuneInfo->sidChipBase(1));
            sid_print(fg(text_color), ", Model = {}{}\n", getModel(tuneInfo->sidModel(1)), panTag(1));
            if (tuneInfo->sidChips() > 2)
            {
                consoleTable(table_t::middle);
                sid_print(fg(label_color), "              : ");
                sid_print(fg(text_color), "3rd SID = ${:0>4X}", tuneInfo->sidChipBase(2));
                sid_print(fg(text_color), ", Model = {}{}\n", getModel(tuneInfo->sidModel(2)), panTag(2));
            }
        }

#ifdef FEAT_SID_V5
        if (showPan)
        {
            static const char* const layouts[] = { "Standard", "LCR", "Center first", "Centered" };
            static const char* const modes[] = { "Direct", "Reverse", "Group", "Spread" };
            const unsigned int layout = static_cast<unsigned int>(tuneInfo->panLayout());
            const unsigned int mode = static_cast<unsigned int>(tuneInfo->panMode());
            consoleTable(table_t::middle);
            sid_print(fg(label_color), " Panning      : ");
            sid_print(fg(text_color), "{}/{}\n",
                (layout < 4) ? layouts[layout] : "Unknown",
                (mode < 4) ? modes[mode] : "Unknown");
        }
#endif

        consoleTable(table_t::separator);

        label_color = (m_iniCfg.console()).label_extra;
        text_color = (m_iniCfg.console()).text_extra;

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Play speed   : ");
        sid_print(fg(text_color), "{}\n", info.speedString());

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " Play mode    : ");
        sid_print(fg(text_color), "{} - {}Hz\n",
                   (m_driver.cfg.channels == 1 ? "Mono" : "Stereo"),
                   m_engCfg.frequency);

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " SID Engine   : ");
        sid_print(fg(text_color), "{} {}\n", getEmu(m_driver.sid),
                   m_filter.enabled ?
#ifdef FEAT_RESID_CAPS
                       ((m_driver.sid == EMU_RESIDFP && m_old6581Caps) ? "(Old 6581 caps)" : "")
#else
                       ""
#endif
                       : "(No Filter)");

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " DigiBoost    : ");
        sid_print(fg(text_color), "{}\n", (m_engCfg.digiBoost ? "Yes" : "No"));

        consoleTable(table_t::middle);
        sid_print(fg(label_color), " SID Model(s) : ");
#ifdef FEAT_SID_MODEL
        {
            std::string models;
            bool same = true;
            for (unsigned int i=0; i<info.numberOfSIDs(); i++)
            {
                if (i)
                    models += ' ';
                models += getModel(info.sidModel(i));
                same = same && info.sidModel(i) == info.sidModel(0);
            }
            // collapse more than 3 identical models to "<N>x <NAME>"
            if (info.numberOfSIDs() > 3 && same)
                models = std::to_string(info.numberOfSIDs()) + "x " + getModel(info.sidModel(0));
            printWrapped(models, label_color, text_color, " ");
        }
#else
        if (m_engCfg.forceSidModel)
            sid_print(fg(text_color), "Forced ");
        else
            sid_print(fg(text_color), "from tune, default = {}\n", getModel(m_engCfg.defaultSidModel));
#endif

        if (m_verboseLevel > 1)
        {
            consoleTable(table_t::middle);
            sid_print(fg(label_color), " Delay        : ");
            sid_print(fg(text_color), "{} (cycles at poweron)\n", info.powerOnDelay());
        }
    }

    const char* romDesc = info.kernalDesc();

    consoleTable(table_t::separator);

    label_color = (m_iniCfg.console()).label_core;
    text_color = (m_iniCfg.console()).text_core;

    consoleTable(table_t::middle);
    sid_print(fg(label_color), " Kernal ROM   : ");
    if (std::strlen(romDesc) == 0)
    {
        sid_print(fg(color_t::red), "None - Some tunes may not play!\n");
    }
    else
    {
        sid_print(fg(text_color), "{}\n", romDesc);
    }

    romDesc = info.basicDesc();

    consoleTable(table_t::middle);
    sid_print(fg(label_color), " BASIC ROM    : ");
    if (std::strlen(romDesc) == 0)
    {
        sid_print(fg(color_t::red), "None - Basic tunes will not play!\n");
    }
    else
    {
        sid_print(fg(text_color), "{}\n", romDesc);
    }

    romDesc = info.chargenDesc();

    consoleTable(table_t::middle);
    sid_print(fg(label_color), " Chargen ROM  : ");
    if (std::strlen(romDesc) == 0)
    {
        sid_print(fg(color_t::red), "None\n");
    }
    else
    {
        sid_print(fg(text_color), "{}\n", romDesc);
    }

    if (m_showhelp)
    {
        consoleTable(table_t::separator);
        consoleTable(table_t::middle);
        sid_print(fg((m_iniCfg.console()).title), " ←/→  Previous/Next  1-9  Toggle voices    p  Pause\n");
        consoleTable(table_t::middle);
        sid_print(fg((m_iniCfg.console()).title), " ↓/↑  Play speed     asd  Toggle samples   h  Help\n");
        consoleTable(table_t::middle);
        sid_print(fg((m_iniCfg.console()).title), " ⇱/⇲  First/Last     f    Toggle filter    q  Quit\n");
    }

#ifdef FEAT_REGS_DUMP_SID
    if (m_verboseLevel > 1)
    {
        consoleTable(table_t::separator);
        consoleTable(table_t::middle);
        fmt::print("         NOTE PW         CONTROL          WAVEFORMS\n");

#ifdef FEAT_NEW_PLAY_API
        for (unsigned int i=0; i < m_engine.installedSIDs() * 3; i++)
#else
        for (int i=0; i < tuneInfo->sidChips() * 3; i++)
#endif
        {
            consoleTable(table_t::middle);
            fmt::print("\n"); // reserve space for each voice's status
        }
    }
#endif

    consoleTable(table_t::end);

    if (m_driver.file)
        fmt::print("Creating audio file, please wait...");
    else
        fmt::print("Playing, press ESC to stop...");

    // Get all the text to the screen so music playback
    // is not disturbed.
    if (m_verboseLevel >= 0)
        fmt::print("00:00");

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

                    sid_print(fg(ctrloff), " Voice {}", (j * 3 + i+1));

                    sid_print(fg((m_iniCfg.console()).notes), " {} ${:0>3X}",
                        getNote(registers[0x00 + i * 0x07] | (registers[0x01 + i * 0x07] << 8)),
                        registers[0x02 + i * 0x07] | ((registers[0x03 + i * 0x07] & 0x0f) << 8));

                    // gate on ?
                    sid_print(fg((oldCtl[i] & 0x01) ? ctrlon : ctrloff), // gate changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x01) ? " GATE" : " gate");

                    // sync on ?
                    sid_print(fg((oldCtl[i] & 0x02) ? ctrlon : ctrloff), // sync changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x02) ? " SYNC" : " sync");

                    // ring on ?
                    sid_print(fg((oldCtl[i] & 0x04) ? ctrlon : ctrloff), // ring changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x04) ? " RING" : " ring");

                    // test on ?
                    sid_print(fg((oldCtl[i] & 0x08) ? ctrlon : ctrloff), // test changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x08) ? " TEST" : " test");

                    // triangle on ?
                    sid_print(fg((oldCtl[i] & 0x10) ? ctrlon : ctrloff), // triangle changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x10) ? " TRI" : " ___");

                    // sawtooth on ?
                    sid_print(fg((oldCtl[i] & 0x20) ? ctrlon : ctrloff), // sawtooth changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x20) ? " SAW" : " ___");

                    // pulse on ?
                    sid_print(fg((oldCtl[i] & 0x40) ? ctrlon : ctrloff), // pulse changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x40) ? " PUL" : " ___");

                    // noise on ?
                    sid_print(fg((oldCtl[i] & 0x80) ? ctrlon : ctrloff), // noise changed ?
                        "{}", (registers[0x04 + i * 0x07] & 0x80) ? " NOI" : " ___");

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

// Display menu outline
void ConsolePlayer::consoleTable(table_t table) const
{
    switch (table)
    {
        case table_t::start:
        sid_print(fg((m_iniCfg.console()).decorations),
                   "{}{}{}",
                   (m_iniCfg.console()).topLeft,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).topRight);
        break;

    case table_t::middle:
        sid_print(fg((m_iniCfg.console()).decorations),
                   "{0}{1}\r{1}",
                   fill{" ", tableWidth+1},
                   (m_iniCfg.console()).vertical);
        return;

    case table_t::separator:
        sid_print(fg((m_iniCfg.console()).decorations),
                   "{}{}{}",
                   (m_iniCfg.console()).junctionRight,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).junctionLeft);
        break;

    case table_t::end:
        sid_print(fg((m_iniCfg.console()).decorations),
                   "{}{}{}",
                   (m_iniCfg.console()).bottomLeft,
                   fill{(m_iniCfg.console()).horizontal, tableWidth},
                   (m_iniCfg.console()).bottomRight);
        break;
    }

    // Move back to begining of row and skip first char
    fmt::print("\n");
}
