/***************************************************************************
                          player.cpp  -  Frontend Player
                             -------------------
    begin                : Sun Oct 7 2001
    copyright            : (C) 2001 by Simon White
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

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <iomanip>
#include "config.h"

#ifdef HAVE_EXCEPTIONS
#   include <new>
#endif

#include "player.h"
#include "keyboard.h"

#ifdef MSVC_HEADER_LOCATIONS
# include <sidbuilder.h>
#else
# include <sidplayfp/sidbuilder.h>
#endif

// Previous song select timeout (3 secs)
#define SID2_PREV_SONG_TIMEOUT 4

#ifdef HAVE_SIDPLAYFP_BUILDERS_RESIDFP_H
#  ifdef MSVC_HEADER_LOCATIONS
#   include <builders/residfp-builder/residfp.h>
#  else
#   include <sidplayfp/builders/residfp.h>
#  endif
const char ConsolePlayer::RESIDFP_ID[]   = "ReSIDfp";
#endif

#ifdef HAVE_SIDPLAYFP_BUILDERS_RESID_H
#  ifdef MSVC_HEADER_LOCATIONS
#   include <builders/resid-builder/resid.h>
#  else
#   include <sidplayfp/builders/resid.h>
#  endif
const char ConsolePlayer::RESID_ID[]   = "ReSID";
#endif

#ifdef HAVE_SIDPLAYFP_BUILDERS_HARDSID_H
#  ifdef MSVC_HEADER_LOCATIONS
#    include <builders/hardsid-builder/hardsid.h>
#  else
#    include <sidplayfp/builders/hardsid.h>
#  endif
const char ConsolePlayer::HARDSID_ID[] = "HardSID";
#endif


ConsolePlayer::ConsolePlayer (const char * const name)
:Event("External Timer\n"),
 m_name(name),
 m_tune(0),
 m_state(playerStopped),
 m_outfile(NULL),
 m_context(NULL),
 m_quietLevel(0),
 m_verboseLevel(0),
 m_cpudebug(false)
{   // Other defaults
    m_filename       = "";
    m_filter.enabled = true;
    m_driver.device  = NULL;
    m_timer.start    = 0;
    m_timer.length   = 0; // FOREVER
    m_timer.valid    = false;
    m_track.first    = 0;
    m_track.selected = 0;
    m_track.loop     = false;
    m_track.single   = false;
    m_speed.current  = 1;
    m_speed.max      = 32;

    // Read default configuration
    m_iniCfg.read ();
    m_engCfg = m_engine.config ();

    {   // Load ini settings
        IniConfig::audio_section     audio     = m_iniCfg.audio();
        IniConfig::emulation_section emulation = m_iniCfg.emulation();

        // INI Configuration Settings
        m_engCfg.clockForced  = emulation.clockForced;
        m_engCfg.clockDefault = emulation.clockSpeed;
        m_engCfg.clockDefault = SID2_CLOCK_PAL;
        m_engCfg.frequency    = audio.frequency;
        m_engCfg.playback     = audio.playback;
        m_precision           = audio.precision;
        m_engCfg.sidDefault   = emulation.sidModel;
        m_engCfg.forceModel   = emulation.forceModel;
        m_engCfg.sidDefault   = SID2_MOS6581;
        m_filter.enabled      = emulation.filter;
        m_filter.bias         = emulation.bias;
        m_filter.filterCurve6581 = emulation.filterCurve6581;
        m_filter.filterCurve8580 = emulation.filterCurve8580;
    }

    createOutput (OUT_NULL, NULL);
    createSidEmu (EMU_NONE);

    kernalRom = loadRom((m_iniCfg.sidplay2()).kernalRom, 8192);
    basicRom = loadRom((m_iniCfg.sidplay2()).basicRom, 8192);
    chargenRom = loadRom((m_iniCfg.sidplay2()).chargenRom, 4096);
    m_engine.setRoms(kernalRom, basicRom, chargenRom);
}

uint8_t* ConsolePlayer::loadRom(const char* romPath, const int size) {
printf("Loading rom %s\n", romPath);
    FILE *file = fopen (romPath, "rb");
    if (!file)
        goto error;
    {
    uint8_t *buffer = new uint8_t[size];
    if (buffer == 0)
        goto error;
    {
    size_t count = fread (buffer, sizeof(char), size, file);
    if (count != size)
        goto error;
    }
    fclose (file);
    return buffer;
    }

error:
printf("Error\n");
    fclose (file);
    return 0;
}

// Create the output object to process sound buffer
bool ConsolePlayer::createOutput (OUTPUTS driver, const SidTuneInfo *tuneInfo)
{
    char *name = NULL;
    const char *title = m_outfile;

    // Remove old audio driver
    m_driver.null.close ();
    m_driver.selected = &m_driver.null;
    if (m_driver.device != NULL)
    {
        if (m_driver.device != &m_driver.null)
            delete m_driver.device;
        m_driver.device = NULL;
    }

    // Create audio driver
    switch (driver)
    {
    case OUT_NULL:
        m_driver.device = &m_driver.null;
        title = "";
    break;

    case OUT_SOUNDCARD:
#ifdef HAVE_EXCEPTIONS
        m_driver.device = new(std::nothrow) AudioDriver;
#else
        m_driver.device = new AudioDriver;
#endif
    break;

    case OUT_WAV:
#ifdef HAVE_EXCEPTIONS
        m_driver.device = new(std::nothrow) WavFile;
#else
        m_driver.device = new WavFile;
#endif
    break;

    default:
        break;
    }

    // Audio driver failed
    if (!m_driver.device)
    {
        m_driver.device = &m_driver.null;
        displayError (ERR_NOT_ENOUGH_MEMORY);
        return false;
    }

    // Generate a name for the wav file
    if (title == NULL)
    {
        size_t length, i;
        title  = tuneInfo->dataFileName();
        length = strlen (title);
        i      = length;
        while (i > 0)
        {
            if (title[--i] == '.')
                break;
        }
        if (!i) i = length;

#ifdef HAVE_EXCEPTIONS
        name = new(std::nothrow) char[i + 10];
#else
        name = new char[i + 10];
#endif
        if (!name)
        {
            displayError (ERR_NOT_ENOUGH_MEMORY);
            return false;
        }

        strcpy (name, title);
        // Prevent extension ".sid.wav"
        name[i] = '\0';

        // Change name based on subtune
        if (tuneInfo->songs() > 1)
            sprintf (&name[i], "[%u]", tuneInfo->currentSong());
        strcat (&name[i], m_driver.device->extension ());
        title = name;
    }

    // Configure with user settings
    m_driver.cfg.frequency = m_engCfg.frequency;
    m_driver.cfg.precision = m_precision;
    m_driver.cfg.channels  = 1; // Mono
    m_driver.cfg.bufSize   = 0; // Recalculate
    if (m_engCfg.playback == sid2_stereo)
        m_driver.cfg.channels = 2;

    {   // Open the hardware
        bool err = false;
        if (m_driver.device->open (m_driver.cfg, title) == NULL)
            err = true;
        // Can't open the same driver twice
        if (driver != OUT_NULL)
        {
            if (m_driver.null.open (m_driver.cfg, title) == NULL)
                err = true;;
        }
        if (name != NULL)
            delete [] name;
        if (err) {
            displayError(m_driver.device->getErrorString());
            return false;
        }
    }

    // See what we got
    m_engCfg.frequency = m_driver.cfg.frequency;
    m_precision = m_driver.cfg.precision;
    switch (m_driver.cfg.channels)
    {
    case 1:
        if (m_engCfg.playback == sid2_stereo)
            m_engCfg.playback  = sid2_mono;
        break;
    case 2:
        if (m_engCfg.playback != sid2_stereo)
            m_engCfg.playback  = sid2_stereo;
        break;
    default:
        cerr << m_name << ": " << "ERROR: " << m_driver.cfg.channels
             << " audio channels not supported" << endl;
        return false;
    }
    return true;
}


// Create the sid emulation
bool ConsolePlayer::createSidEmu (SIDEMUS emu)
{
    // Remove old driver and emulation
    if (m_engCfg.sidEmulation)
    {
        sidbuilder *builder   = m_engCfg.sidEmulation;
        m_engCfg.sidEmulation = NULL;
        m_engine.config (m_engCfg);
        delete builder;
    }

    // Now setup the sid emulation
    switch (emu)
    {
#ifdef HAVE_SIDPLAYFP_BUILDERS_RESIDFP_H
    case EMU_RESIDFP:
    {
#ifdef HAVE_EXCEPTIONS
        ReSIDfpBuilder *rs = new(std::nothrow) ReSIDfpBuilder( RESIDFP_ID );
#else
        ReSIDfpBuilder *rs = new ReSIDfpBuilder( RESIDFP_ID );
#endif
        if (rs)
        {
            m_engCfg.sidEmulation = rs;
            if (!rs->getStatus()) goto createSidEmu_error;
            rs->create ((m_engine.info ()).maxsids);
            if (!rs->getStatus()) goto createSidEmu_error;

            if (m_filter.filterCurve6581)
                rs->filter6581Curve(m_filter.filterCurve6581);
            if (m_filter.filterCurve8580)
                rs->filter8580Curve((double)m_filter.filterCurve8580);
        }
        break;
    }
#endif // HAVE_SIDPLAYFP_BUILDERS_RESIDFP_H

#ifdef HAVE_SIDPLAYFP_BUILDERS_RESID_H
    case EMU_RESID:
    {
#ifdef HAVE_EXCEPTIONS
        ReSIDBuilder *rs = new(std::nothrow) ReSIDBuilder( RESID_ID );
#else
        ReSIDBuilder *rs = new ReSIDfpBuilder( RESID_ID );
#endif
        if (rs)
        {
            m_engCfg.sidEmulation = rs;
            if (!rs->getStatus()) goto createSidEmu_error;
            rs->create ((m_engine.info ()).maxsids);
            if (!rs->getStatus()) goto createSidEmu_error;

            rs->bias(m_filter.bias);
        }
        break;
    }
#endif // HAVE_SIDPLAYFP_BUILDERS_RESID_H

#ifdef HAVE_SIDPLAYFP_BUILDERS_HARDSID_H
    case EMU_HARDSID:
    {
#ifdef HAVE_EXCEPTIONS
        HardSIDBuilder *hs = new(std::nothrow) HardSIDBuilder( HARDSID_ID );
#else
        HardSIDBuilder *hs = new HardSIDBuilder( HARDSID_ID );
#endif
        if (hs)
        {
            m_engCfg.sidEmulation = hs;
            if (!hs->getStatus()) goto createSidEmu_error;
            hs->create ((m_engine.info ()).maxsids);
            if (!hs->getStatus()) goto createSidEmu_error;
        }
        break;
    }
#endif // HAVE_SIDPLAYFP_BUILDERS_HARDSID_H

    default:
        // Emulation Not yet handled
        // This default case results in the default
        // emulation
        break;
    }

    if (!m_engCfg.sidEmulation)
    {
        if (emu > EMU_DEFAULT)
        {   // No sid emulation?
            displayError (ERR_NOT_ENOUGH_MEMORY);
            return false;
        }
    }

    if (m_engCfg.sidEmulation) {
        /* set up SID filter. HardSID just ignores call with def. */
        m_engCfg.sidEmulation->filter(m_filter.enabled);
    }

    return true;

createSidEmu_error:
    displayError (m_engCfg.sidEmulation->error ());
    delete m_engCfg.sidEmulation;
    m_engCfg.sidEmulation = NULL;
    return false;
}


bool ConsolePlayer::open (void)
{
    if ((m_state & ~playerFast) == playerRestart)
    {
        if (m_quietLevel < 2)
            cerr << endl;
        if (m_state & playerFast)
            m_driver.selected->reset ();
        m_state = playerStopped;
    }

    // Select the required song
    m_track.selected = m_tune.selectSong (m_track.selected);
    if (m_engine.load (&m_tune) < 0)
    {
        displayError (m_engine.error ());
        return false;
    }

    // Get tune details
    const SidTuneInfo *tuneInfo = m_tune.getInfo ();
    if (!m_track.single)
        m_track.songs = tuneInfo->songs();
    if (!createOutput (m_driver.output, tuneInfo))
        return false;
    if (!createSidEmu (m_driver.sid))
        return false;

    // Configure engine with settings
    if (m_engine.config (m_engCfg) < 0)
    {   // Config failed
        displayError (m_engine.error ());
        return false;
    }

    // Start the player.  Do this by fast
    // forwarding to the start position
    m_driver.selected = &m_driver.null;
    m_speed.current   = m_speed.max;
    m_engine.fastForward (100 * m_speed.current);
    v1mute = v2mute = v3mute = false;

    // As yet we don't have a required songlength
    // so try the songlength database
    if (!m_timer.valid)
    {
        int_least32_t length = m_database.length (m_tune);
        if (length > 0)
            m_timer.length = length;
    }

    // Set up the play timer
    m_context = m_engine.getEventContext();
    m_timer.stop  = 0;
    m_timer.stop += m_timer.length;

    if (m_timer.valid)
    {   // Length relative to start
        m_timer.stop += m_timer.start;
    }
    else
    {   // Check to make start time dosen't exceed end
        if (m_timer.stop & (m_timer.start >= m_timer.stop))
        {
            displayError ("ERROR: Start time exceeds song length!");
            return false;
        }
    }

    m_timer.current = ~0;
    m_state = playerRunning;

    // Update display
    menu  ();
    event ();
    return true;
}

void ConsolePlayer::close ()
{
    m_engine.stop   ();
    if (m_state == playerExit)
    {   // Natural finish
        emuflush ();
        if (m_driver.file)
            cerr << (char) 7; // Bell
    }
    else // Destroy buffers
        m_driver.selected->reset ();

    // Shutdown drivers, etc
    createOutput    (OUT_NULL, NULL);
    createSidEmu    (EMU_NONE);
    m_engine.load   (NULL);
    m_engine.config (m_engCfg);

    if (m_quietLevel < 2)
    {   // Correctly leave ansi mode and get prompt to
        // end up in a suitable location
        if ((m_iniCfg.console ()).ansi)
            cerr << '\x1b' << "[0m";
#ifndef HAVE_MSWINDOWS
        cerr << endl;
#endif
    }
}

// Flush any hardware sid fifos so all music is played
void ConsolePlayer::emuflush ()
{
    switch (m_driver.sid)
    {
#ifdef HAVE_SIDPLAYFP_BUILDERS_HARDSID_H
    case EMU_HARDSID:
        ((HardSIDBuilder *)m_engCfg.sidEmulation)->flush ();
        break;
#endif // HAVE_LIBHARDSID_BUILDER
    default:
        break;
    }
}


// Out play loop to be externally called
bool ConsolePlayer::play ()
{
    short *buffer = m_driver.selected->buffer ();
    uint_least32_t length = m_driver.cfg.bufSize;

    if (m_state == playerRunning)
    {
        // Fill buffer
        uint_least32_t ret;
        ret = m_engine.play (buffer, length);
        if (ret < length)
        {
            if (m_engine.isPlaying ())
            {
                m_state = playerError;
                return false;
            }
            return false;
        }
    }

    switch (m_state)
    {
    case playerRunning:
        m_driver.selected->write ();
        // Deliberate run on
    case playerPaused:
        // Check for a keypress (approx 250ms rate, but really depends
        // on music buffer sizes).  Don't do this for high quiet levels
        // as chances are we are under remote control.
        if ((m_quietLevel < 2) && _kbhit ())
            decodeKeys ();
        return true;
    default:
        if (m_quietLevel < 2)
            cerr << endl;
        m_engine.stop ();
#if HAVE_TSID == 1
        if (m_tsid)
        {
            m_tsid.addTime ((int) m_timer.current, m_track.selected,
                            m_filename);
        }
#elif HAVE_TSID == 2
        if (m_tsid)
        {
            int_least32_t length;
            char md5[SidTune::MD5_LENGTH + 1];
            m_tune.createMD5 (md5);
            length = m_database.length (md5, m_track.selected);
            // ignore errors
            if (length < 0)
                length = 0;
            m_tsid.addTime (md5, m_filename, (uint) m_timer.current,
                            m_track.selected, (uint) length);
        }
#endif
        break;
    }
    return false;
}


void ConsolePlayer::stop ()
{
    m_state = playerStopped;
    m_engine.stop ();
}


// External Timer Event
void ConsolePlayer::event (void)
{
    uint_least32_t seconds = m_engine.time();
    if ( !m_quietLevel )
    {
        cerr << "\b\b\b\b\b" << std::setw(2) << std::setfill('0')
             << ((seconds / 60) % 100) << ':' << std::setw(2)
             << std::setfill('0') << (seconds % 60) << std::flush;
    }

    if (seconds != m_timer.current)
    {
        m_timer.current = seconds;

        if (seconds == m_timer.start)
        {   // Switch audio drivers.
            m_driver.selected = m_driver.device;
            memset (m_driver.selected->buffer (), 0, m_driver.cfg.bufSize);
            m_speed.current = 1;
            m_engine.fastForward (100);
            if (m_cpudebug)
                m_engine.debug (true, NULL);
        }
        else if (m_timer.stop && (seconds == m_timer.stop))
        {
            m_state = playerExit;
            for (;;)
            {
                if (m_track.single)
                    return;
                // Move to next track
                m_track.selected++;
                if (m_track.selected > m_track.songs)
                    m_track.selected = 1;
                if (m_track.selected == m_track.first)
                    return;
                m_state = playerRestart;
                break;
            }
            if (m_track.loop)
                m_state = playerRestart;
        }
    }

    // Units in C64 clock cycles
    m_context->schedule (*this, 900000, EVENT_CLOCK_PHI1);
}


void ConsolePlayer::displayError (const char *error)
{
    cerr << m_name << ": " << error << endl;
}


// Keyboard handling
void ConsolePlayer::decodeKeys ()
{
    int action;

    do
    {
        action = keyboard_decode ();
        if (action == A_INVALID)
            continue;

        switch (action)
        {
        case A_RIGHT_ARROW:
            m_state = playerFastRestart;
            if (!m_track.single)
            {
                m_track.selected++;
                if (m_track.selected > m_track.songs)
                    m_track.selected = 1;
            }
        break;

        case A_LEFT_ARROW:
            m_state = playerFastRestart;
            if (!m_track.single)
            {   // Only select previous song if less than timeout
                // else restart current song
                if ((m_engine.time()) < SID2_PREV_SONG_TIMEOUT)
                {
                    m_track.selected--;
                    if (m_track.selected < 1)
                        m_track.selected = m_track.songs;
                }
            }
        break;

        case A_UP_ARROW:
            m_speed.current *= 2;
            if (m_speed.current > m_speed.max)
                m_speed.current = m_speed.max;
            m_engine.fastForward (100 * m_speed.current);
        break;

        case A_DOWN_ARROW:
            m_speed.current = 1;
            m_engine.fastForward (100);
        break;

        case A_HOME:
            m_state = playerFastRestart;
            m_track.selected = 1;
        break;

        case A_END:
            m_state = playerFastRestart;
            m_track.selected = m_track.songs;
        break;

        case A_PAUSED:
            if (m_state == playerPaused)
            {
                cerr << "\b\b\b\b\b\b\b\b\b";
                // Just to make sure PAUSED is removed from screen
                cerr << "         ";
                cerr << "\b\b\b\b\b\b\b\b\b";
                m_state  = playerRunning;
            }
            else
            {
                cerr << " [PAUSED]";
                m_state = playerPaused;
                m_driver.selected->pause ();
            }
        break;

        case A_TOGGLE_VOICE1:
            v1mute = !v1mute;
            m_engine.mute(0, 0, v1mute);
        break;

        case A_TOGGLE_VOICE2:
            v2mute = !v2mute;
            m_engine.mute(0, 1, v2mute);
        break;

        case A_TOGGLE_VOICE3:
            v3mute = !v3mute;
            m_engine.mute(0, 2, v3mute);
        break;
        //TODO add mute for second SID

        case A_TOGGLE_FILTER:
            m_filter.enabled = !m_filter.enabled;
            m_engCfg.sidEmulation->filter(m_filter.enabled);
        break;

        case A_QUIT:
            m_state = playerFastExit;
            return;
        break;
        }
    } while (_kbhit ());
}
