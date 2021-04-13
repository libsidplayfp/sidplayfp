/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2011-2021 Leandro Nini
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

#include <cstring>
#include <ctype.h>

#include <iostream>
#include <iomanip>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::dec;
using std::hex;
using std::flush;
using std::setw;
using std::setfill;
using std::string;

#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>

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
#ifdef FEAT_REGS_DUMP_SID
const char *ConsolePlayer::getNote(uint16_t freq)
{
    if (freq)
    {
        int distance = 0xffff;
        for (int i=0; i<(12 * 8); i++)
        {
            int d = abs(freq - m_freqTable[i]);
            if (d < distance)
                distance = d;
            else
                return noteName[i];
        }
    }

    return "---";
}
#endif
// Display console menu
void ConsolePlayer::menu ()
{
    if (m_quietLevel > 1)
        return;

    const SidInfo &info         = m_engine.info ();
    const SidTuneInfo *tuneInfo = m_tune.getInfo();

    // cerr << (char) 12 << '\f'; // New Page
    if ((m_iniCfg.console ()).ansi)
    {
        cerr << '\x1b' << "[40m";  // Background black
        cerr << '\x1b' << "[2J";   // Clear screen
        cerr << '\x1b' << "[0;0H"; // Move cursor to 0,0
    }

    consoleTable (tableStart);
    consoleTable (tableMiddle);
    consoleColour (red, true);
    cerr << "  SID";
    consoleColour (blue, true);
    cerr << "PLAYFP";
    consoleColour (white, true);
    cerr << " - Music Player and C64 SID Chip Emulator" << endl;
    consoleTable  (tableMiddle);
    consoleColour (white, false);
    {
        string version;
        version.reserve(54);
        version.append("Sidplayfp V" VERSION ", ").append(1, toupper(*info.name())).append(info.name() + 1).append(" V").append(info.version());
        cerr << setw(54/2 + version.length()/2) << version << endl;
    }

    const unsigned int n = tuneInfo->numberOfInfoStrings();
    if (n)
    {
        codeConvert codeset;

        consoleTable (tableSeparator);

        consoleTable  (tableMiddle);
        consoleColour (cyan, true);
        cerr << " Title        : ";
        consoleColour (magenta, true);
        cerr << codeset.convert(tuneInfo->infoString(0)) << endl;
        if (n>1)
        {
            consoleTable  (tableMiddle);
            consoleColour (cyan, true);
            cerr << " Author       : ";
            consoleColour (magenta, true);
            cerr << codeset.convert(tuneInfo->infoString(1)) << endl;
            consoleTable  (tableMiddle);
            consoleColour (cyan, true);
            cerr << " Released     : ";
            consoleColour (magenta, true);
            cerr << codeset.convert(tuneInfo->infoString(2)) << endl;
        }
    }

    for (unsigned int i = 0; i < tuneInfo->numberOfCommentStrings(); i++)
    {
        consoleTable  (tableMiddle);
        consoleColour (cyan, true);
        cerr << " Comment      : ";
        consoleColour (magenta, true);
        cerr << tuneInfo->commentString(i) << endl;
    }

    consoleTable (tableSeparator);

    if (m_verboseLevel)
    {
        consoleTable  (tableMiddle);
        consoleColour (green, true);
        cerr << " File format  : ";
        consoleColour (white, true);
        cerr << tuneInfo->formatString() << endl;
        consoleTable  (tableMiddle);
        consoleColour (green, true);
        cerr << " Filename(s)  : ";
        consoleColour (white, true);
        cerr << tuneInfo->dataFileName() << endl;
        // Second file is only sometimes present
        if (tuneInfo->infoFileName())
        {
            consoleTable  (tableMiddle);
            consoleColour (green, true);
            cerr << "              : ";
            consoleColour (white, true);
            cerr << tuneInfo->infoFileName() << endl;
        }
        consoleTable  (tableMiddle);
        consoleColour (green, true);
        cerr << " Condition    : ";
        consoleColour (white, true);
        cerr << m_tune.statusString() << endl;

#if HAVE_TSID == 1
        if (!m_tsid)
        {
            consoleTable  (tableMiddle);
            consoleColour (green, true);
            cerr << " TSID Error   : ";
            consoleColour (white, true);
            cerr << m_tsid.getError () << endl;
        }
#endif // HAVE_TSID
    }

    consoleTable  (tableMiddle);
    consoleColour (green, true);
    cerr << " Playlist     : ";
    consoleColour (white, true);

    {   // This will be the format used for playlists
        int i = 1;
        if (!m_track.single)
        {
            i  = m_track.selected;
            i -= (m_track.first - 1);
            if (i < 1)
                i += m_track.songs;
        }
        cerr << i << '/' << m_track.songs;
        cerr << " (tune " << tuneInfo->currentSong() << '/'
             << tuneInfo->songs() << '['
             << tuneInfo->startSong() << "])";
    }

    if (m_track.loop)
        cerr << " [LOOPING]";
    cerr << endl;

    if (m_verboseLevel)
    {
        consoleTable  (tableMiddle);
        consoleColour (green, true);
        cerr << " Song Speed   : ";
        consoleColour (white, true);
        cerr << getClock(tuneInfo->clockSpeed()) << endl;
    }

    consoleTable  (tableMiddle);
    consoleColour (green, true);
    cerr << " Song Length  : ";
    consoleColour (white, true);
    if (m_timer.stop)
    {
        const uint_least32_t seconds = m_timer.stop / 1000;
        cerr << setw(2) << setfill('0') << ((seconds / 60) % 100)
             << ':' << setw(2) << setfill('0') << (seconds % 60);
#ifdef FEAT_NEW_SONLEGTH_DB
        cerr << '.' << setw(3) << m_timer.stop % 1000;
#endif
    }
    else if (m_timer.valid)
        cerr << "FOREVER";
    else
        cerr << "UNKNOWN";
    if (m_timer.start)
    {   // Show offset
        const uint_least32_t seconds = m_timer.start / 1000;
        cerr << " (+" << setw(2) << setfill('0') << ((seconds / 60) % 100)
             << ':' << setw(2) << setfill('0') << (seconds % 60) << ")";
    }
    cerr << endl;

    if (m_verboseLevel)
    {
        consoleTable  (tableSeparator);

        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " Addresses    : " << hex;
        cerr.setf(std::ios::uppercase);
        consoleColour (white, false);
        // Display PSID Driver location
        cerr << "DRIVER = ";
        if (info.driverAddr() == 0)
            cerr << "NOT PRESENT";
        else
        {
            cerr << "$"  << setw(4) << setfill('0') << info.driverAddr();
            cerr << "-$" << setw(4) << setfill('0') << info.driverAddr() +
                (info.driverLength() - 1);
        }
        if (tuneInfo->playAddr() == 0xffff)
            cerr << ", SYS = $" << setw(4) << setfill('0') << tuneInfo->initAddr();
        else
            cerr << ", INIT = $" << setw(4) << setfill('0') << tuneInfo->initAddr();
        cerr << endl;
        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << "              : ";
        consoleColour (white, false);
        cerr << "LOAD   = $" << setw(4) << setfill('0') << tuneInfo->loadAddr();
        cerr << "-$"         << setw(4) << setfill('0') << tuneInfo->loadAddr() +
            (tuneInfo->c64dataLen() - 1);
        if (tuneInfo->playAddr() != 0xffff)
            cerr << ", PLAY = $" << setw(4) << setfill('0') << tuneInfo->playAddr();
        cerr << dec << endl;
        cerr.unsetf(std::ios::uppercase);

        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " SID Details  : ";
        consoleColour (white, false);
        cerr << "Model = ";
#ifdef FEAT_NEW_TUNEINFO_API
        cerr << getModel(tuneInfo->sidModel(0));
#else
        cerr << getModel(tuneInfo->sidModel1());
#endif
        cerr << endl;
#ifdef FEAT_NEW_TUNEINFO_API
        if (tuneInfo->sidChips() > 1)
#else
        if (tuneInfo->isStereo())
#endif
        {
            consoleTable  (tableMiddle);
            consoleColour (yellow, true);
            cerr << "              : ";
            consoleColour (white, false);
#ifdef FEAT_NEW_TUNEINFO_API
            cerr << "2nd SID = $" << hex << tuneInfo->sidChipBase(1) << dec;
            cerr << ", Model = " << getModel(tuneInfo->sidModel(1));
#else
            cerr << "2nd SID = $" << hex << tuneInfo->sidChipBase2() << dec;
            cerr << ", Model = " << getModel(tuneInfo->sidModel2());
#endif
            cerr << endl;
#ifdef FEAT_NEW_TUNEINFO_API
            if (tuneInfo->sidChips() > 2)
            {
                consoleTable  (tableMiddle);
                consoleColour (yellow, true);
                cerr << "              : ";
                consoleColour (white, false);
                cerr << "3rd SID = $" << hex << tuneInfo->sidChipBase(2) << dec;
                cerr << ", Model = " << getModel(tuneInfo->sidModel(2));
                cerr << endl;
            }
#endif
        }

        consoleTable  (tableSeparator);

        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " Play speed   : ";
        consoleColour (white, false);
        cerr << info.speedString() << endl;

        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " Play mode    : ";
        consoleColour (white, false);
        cerr << (info.channels() == 1 ? "Mono" : "Stereo") << endl;

        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " SID Filter   : ";
        consoleColour (white, false);
        cerr << (m_filter.enabled ? "Yes" : "No") << endl;
#ifdef FEAT_DIGIBOOST
        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " DigiBoost    : ";
        consoleColour (white, false);
        cerr << (m_engCfg.digiBoost ? "Yes" : "No") << endl;
#endif
        consoleTable  (tableMiddle);
        consoleColour (yellow, true);
        cerr << " SID Model    : ";
        consoleColour (white, false);
        if (m_engCfg.forceSidModel)
            cerr << "Forced ";
        else
            cerr << "from tune, default = ";
        cerr << getModel(m_engCfg.defaultSidModel) << endl;

        if (m_verboseLevel > 1)
        {
            consoleTable  (tableMiddle);
            consoleColour (yellow, true);
            cerr << " Delay        : ";
            consoleColour (white, false);
            cerr << info.powerOnDelay() << " (cycles at poweron)" << endl;
        }
    }

    const char* romDesc = info.kernalDesc();

    consoleTable  (tableSeparator);

    consoleTable  (tableMiddle);
    consoleColour (magenta, true);
    cerr << " Kernal ROM   : ";
    if (strlen(romDesc) == 0)
    {
        consoleColour (red, false);
        cerr << "None - Some tunes may not play!";
    }
    else
    {
        consoleColour (white, false);
        cerr << romDesc;
    }
    cerr << endl;

    romDesc = info.basicDesc();

    consoleTable  (tableMiddle);
    consoleColour (magenta, true);
    cerr << " BASIC ROM    : ";
    if (strlen(romDesc) == 0)
    {
        consoleColour (red, false);
        cerr << "None - Basic tunes will not play!";
    }
    else
    {
        consoleColour (white, false);
        cerr << romDesc;
    }
    cerr << endl;

    romDesc = info.chargenDesc();

    consoleTable  (tableMiddle);
    consoleColour (magenta, true);
    cerr << " Chargen ROM  : ";
    if (strlen(romDesc) == 0)
    {
        consoleColour (red, false);
        cerr << "None";
    }
    else
    {
        consoleColour (white, false);
        cerr << romDesc;
    }
    cerr << endl;

#ifdef FEAT_REGS_DUMP_SID
    if (m_verboseLevel > 1)
    {
        consoleTable  (tableSeparator);
        consoleTable (tableMiddle); cerr << "         NOTE PW         CONTROL          WAVEFORMS" << endl;

        for (int i=0; i < tuneInfo->sidChips() * 3; i++)
        {
            consoleTable (tableMiddle); cerr << endl; // reserve space for the Voice 3 status
        }
    }
#endif

    consoleTable (tableEnd);

    if (m_driver.file)
        cerr << "Creating audio file, please wait...";
    else
        cerr << "Playing, press ESC to stop...";

    // Get all the text to the screen so music playback
    // is not disturbed.
    if ( !m_quietLevel )
        cerr << "00:00";
    cerr << flush;
}

void ConsolePlayer::refreshRegDump()
{
#ifdef FEAT_REGS_DUMP_SID
    if (m_verboseLevel > 1)
    {
        const SidTuneInfo *tuneInfo = m_tune.getInfo();

        cerr << "\x1b[" << tuneInfo->sidChips() * 3 + 1 << "A\r"; // Moves cursor X lines up

        for (int j=0; j < tuneInfo->sidChips(); j++)
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
                    consoleTable (tableMiddle);
                    consoleColour (red, true);

                    cerr << " Voice " << (j * 3 + i+1) << hex;

                    consoleColour (yellow, true);
                    ;
                    cerr << " " << getNote(registers[0x00 + i * 0x07] | ((registers[0x01 + i * 0x07] & 0x0f) << 8))
                         << " $" << setw(3) << setfill('0') << (registers[0x02 + i * 0x07] | ((registers[0x03 + i * 0x07] & 0x0f) << 8));

                    // gate changed ?
                    consoleColour((oldCtl[i] & 0x01) ? green : red, true);
                    // gate on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x01) ? " GATE" : " gate");

                    // sync changed ?
                    consoleColour((oldCtl[i] & 0x02) ? green : red, true);
                    // sync on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x02) ? " SYNC" : " sync");

                    // ring changed ?
                    consoleColour((oldCtl[i] & 0x04) ? green : red, true);
                    // ring on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x04) ? " RING" : " ring");

                    // test changed ?
                    consoleColour((oldCtl[i] & 0x08) ? green : red, true);
                    // test on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x08) ? " TEST" : " test");

                    // triangle changed ?
                    consoleColour((oldCtl[i] & 0x10) ? green : red, true);
                    // triangle on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x10) ? " TRI" : " ___");

                    // sawtooth changed ?
                    consoleColour((oldCtl[i] & 0x20) ? green : red, true);
                    // sawtooth on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x20) ? " SAW" : " ___");

                    // pulse changed ?
                    consoleColour((oldCtl[i] & 0x40) ? green : red, true);
                    // pulse on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x40) ? " PUL" : " ___");

                    // noise changed ?
                    consoleColour((oldCtl[i] & 0x80) ? green : red, true);
                    // noise on ?
                    cerr << ((registers[0x04 + i * 0x07] & 0x80) ? " NOI" : " ___");

                    cerr << dec << endl;
                }
            }
            else
            {
                consoleTable (tableMiddle); cerr << "???" << endl;
                consoleTable (tableMiddle); cerr << "???" << endl;
                consoleTable (tableMiddle); cerr << "???" << endl;
            }
        }

        consoleTable (tableEnd);
    }
    else
#endif
        cerr << "\r";

    if (m_driver.file)
        cerr << "Creating audio file, please wait...";
    else
        cerr << "Playing, press ESC to stop...";

    cerr << flush;
}

// Set colour of text on console
void ConsolePlayer::consoleColour (player_colour_t colour, bool bold)
{
    if ((m_iniCfg.console ()).ansi)
    {
        const char *mode = "";

        switch (colour)
        {
        case black:   mode = "30"; break;
        case red:     mode = "31"; break;
        case green:   mode = "32"; break;
        case yellow:  mode = "33"; break;
        case blue:    mode = "34"; break;
        case magenta: mode = "35"; break;
        case cyan:    mode = "36"; break;
        case white:   mode = "37"; break;
        }

        if (bold)
            cerr << '\x1b' << "[1;40;" << mode << 'm';
        else
            cerr << '\x1b' << "[0;40;" << mode << 'm';
    }
}

// Display menu outline
void ConsolePlayer::consoleTable (player_table_t table)
{
    const unsigned int tableWidth = 54;

    consoleColour (white, true);
    switch (table)
    {
    case tableStart:
        cerr << (m_iniCfg.console ()).topLeft << setw(tableWidth)
             << setfill ((m_iniCfg.console ()).horizontal) << ""
             << (m_iniCfg.console ()).topRight;
        break;

    case tableMiddle:
        cerr << setw(tableWidth + 1) << setfill(' ') << ""
             << (m_iniCfg.console ()).vertical << '\r'
             << (m_iniCfg.console ()).vertical;
        return;

    case tableSeparator:
        cerr << (m_iniCfg.console ()).junctionRight << setw(tableWidth)
             << setfill ((m_iniCfg.console ()).horizontal) << ""
             << (m_iniCfg.console ()).junctionLeft;
        break;

    case tableEnd:
        cerr << (m_iniCfg.console ()).bottomLeft << setw(tableWidth)
             << setfill ((m_iniCfg.console ()).horizontal) << ""
             << (m_iniCfg.console ()).bottomRight;
        break;
    }

    // Move back to begining of row and skip first char
    cerr << "\n";
}


// Restore Ansi Console to defaults
void ConsolePlayer::consoleRestore ()
{
    if ((m_iniCfg.console ()).ansi)
        cerr << '\x1b' << "[0m";
}
