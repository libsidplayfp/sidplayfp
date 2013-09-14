/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2013 Leandro Nini
 * Copyright 2001-2002 Simon White
 * Copyright 2000 Jarno Paananen
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

#include "audiodrv.h"

#ifdef HAVE_MMSYSTEM

#include <stdio.h>
#include <mmreg.h>

Audio_MMSystem::Audio_MMSystem() :
    AudioBase("MMSYSTEM"),
    waveHandle(0),
    isOpen(false)
{
    for ( int i = 0; i < MAXBUFBLOCKS; i++ )
    {
        blockHeaderHandles[i] = 0;
        blockHandles[i]       = 0;
        blockHeaders[i]       = NULL;
        blocks[i]             = NULL;
    }
}

Audio_MMSystem::~Audio_MMSystem()
{
    close();
}

bool Audio_MMSystem::open(AudioConfig &cfg)
{
    WAVEFORMATEX  wfm;

    if (isOpen)
    {
        setError("Audio device already open.");
        return false;
    }
    isOpen = true;

    /* Initialise blocks */
    memset (blockHandles, 0, sizeof (blockHandles));
    memset (blockHeaders, 0, sizeof (blockHeaders));
    memset (blockHeaderHandles, 0, sizeof (blockHeaderHandles));

    // Format
    memset (&wfm, 0, sizeof(WAVEFORMATEX));
    wfm.wFormatTag      = WAVE_FORMAT_PCM;
    wfm.nChannels       = cfg.channels;
    wfm.nSamplesPerSec  = cfg.frequency;
    wfm.wBitsPerSample  = 16;
    wfm.nBlockAlign     = wfm.wBitsPerSample / 8 * wfm.nChannels;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nBlockAlign;
    wfm.cbSize          = 0;

    // Rev 1.3 (saw) - Calculate buffer to hold 250ms of data
    bufSize = wfm.nSamplesPerSec / 4 * wfm.nBlockAlign;

    try
    {
        cfg.bufSize = bufSize / 2;
        waveOutOpen (&waveHandle, WAVE_MAPPER, &wfm, 0, 0, 0);
        if ( !waveHandle )
        {
            throw error("Can't open wave out device.");
        }

        _settings = cfg;

        {
            /* Allocate and lock memory for all mixing blocks: */
            for (int i = 0; i < MAXBUFBLOCKS; i++ )
            {
                /* Allocate global memory for mixing block: */
                if ( (blockHandles[i] = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
                                                    bufSize)) == NULL )
                {
                    throw error("Can't allocate global memory.");
                }

                /* Lock mixing block memory: */
                if ( (blocks[i] = (short *)GlobalLock(blockHandles[i])) == NULL )
                {
                    throw error("Can't lock global memory.");
                }

                /* Allocate global memory for mixing block header: */
                if ( (blockHeaderHandles[i] = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
                                                        sizeof(WAVEHDR))) == NULL )
                {
                    throw error("Can't allocate global memory.");
                }

                /* Lock mixing block header memory: */
                WAVEHDR *header;
                if ( (header = blockHeaders[i] =
                    (WAVEHDR*)GlobalLock(blockHeaderHandles[i])) == NULL )
                {
                    throw error("Can't lock global memory.");
                }

                /* Reset wave header fields: */
                memset (header, 0, sizeof (WAVEHDR));
                header->lpData         = (char*)blocks[i];
                header->dwBufferLength = bufSize;
                header->dwFlags        = WHDR_DONE; /* mark the block is done */
            }
        }

        blockNum = 0;
        _sampleBuffer = blocks[blockNum];
        return true;
    }
    catch(error const &e)
    {
        setError(e.message());

        close ();
        return false;
    }
}

bool Audio_MMSystem::write()
{
    if (!isOpen)
    {
        setError("Device not open.");
        return false;
    }

    /* Reset wave header fields: */
    blockHeaders[blockNum]->dwFlags = 0;

    /* Prepare block header: */
    if ( waveOutPrepareHeader(waveHandle, blockHeaders[blockNum],
                              sizeof(WAVEHDR)) != MMSYSERR_NOERROR )
    {
        setError("Error in waveOutPrepareHeader.");
        return false;
    }

    if ( waveOutWrite(waveHandle, blockHeaders[blockNum],
                      sizeof(WAVEHDR)) != MMSYSERR_NOERROR )
    {
        setError("Error in waveOutWrite.");
        return false;
    }

    /* Next block, circular buffer style, and I don't like modulo. */
    blockNum++;
    blockNum %= MAXBUFBLOCKS;

    /* Wait for the next block to become free */
    while ( !(blockHeaders[blockNum]->dwFlags & WHDR_DONE) )
        Sleep(20);

    if ( waveOutUnprepareHeader(waveHandle, blockHeaders[blockNum],
                                sizeof(WAVEHDR)) != MMSYSERR_NOERROR )
    {
        setError("Error in waveOutUnprepareHeader.");
        return false;
    }

    _sampleBuffer = blocks[blockNum];
    return true;
}

// Rev 1.2 (saw) - Changed, see AudioBase.h
void Audio_MMSystem::reset()
{
    if (!isOpen)
        return;

    // Stop play and kill the current music.
    // Start new music data being added at the begining of
    // the first buffer
    if ( waveOutReset(waveHandle) != MMSYSERR_NOERROR )
    {
        setError("Error in waveOutReset.");
        return;
    }
    blockNum = 0;
    _sampleBuffer = blocks[blockNum];
}

void Audio_MMSystem::close()
{
    if ( !isOpen )
        return;

    isOpen        = false;
    _sampleBuffer = NULL;

    /* Reset wave output device, stop playback, and mark all blocks done: */
    if ( waveHandle )
    {
        waveOutReset(waveHandle);

        /* Make sure all blocks are indeed done: */
        int doneTimeout = 500;

        for (;;) {
            bool allDone = true;
            for ( int i = 0; i < MAXBUFBLOCKS; i++ ) {
                if ( blockHeaders[i] && (blockHeaders[i]->dwFlags & WHDR_DONE) == 0 )
                    allDone = false;
            }

            if ( allDone || (doneTimeout == 0) )
                break;
            doneTimeout--;
            Sleep(20);
        }

        /* Unprepare all mixing blocks, unlock and deallocate
           all mixing blocks and mixing block headers: */
        for ( int i = 0; i < MAXBUFBLOCKS; i++ )
        {
            if ( blockHeaders[i] )
                waveOutUnprepareHeader(waveHandle, blockHeaders[i], sizeof(WAVEHDR));

            if ( blockHeaderHandles[i] )
            {
                GlobalUnlock(blockHeaderHandles[i]);
                GlobalFree(blockHeaderHandles[i]);
            }
            if ( blockHandles[i] )
            {
                GlobalUnlock(blockHandles[i]);
                GlobalFree(blockHandles[i]);
            }
        }

        /* Close wave output device: */
        waveOutClose(waveHandle);
    }
}

#endif // HAVE_MMSYSTEM
