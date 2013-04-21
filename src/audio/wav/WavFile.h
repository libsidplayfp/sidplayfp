/*
 * This file is part of sidplayfp, a SID player.
 *
 * Copyright 2011-2013 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2000-2004 Simon White
 * Copyright 2000 Michael Schwendt
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

#ifndef WAV_FILE_HEADER_H
#define WAV_FILE_HEADER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include "../AudioBase.h"

struct wavHeader                        // little endian format
{
    char mainChunkID[4];                // 'RIFF' (ASCII)

    unsigned char length[4];            // file length

    char chunkID[4];                    // 'WAVE' (ASCII)
    char subChunkID[4];                    // 'fmt ' (ASCII)
    char subChunkLen[4];                // length of subChunk, always 16 bytes
    unsigned char format[2];            // currently always = 1 = PCM-Code

    unsigned char channels[2];            // 1 = mono, 2 = stereo
    unsigned char sampleFreq[4];        // sample-frequency
    unsigned char bytesPerSec[4];        // sampleFreq * blockAlign
    unsigned char blockAlign[2];        // bytes per sample * channels
    unsigned char bitsPerSample[2];

    char dataChunkID[4];                // keyword, begin of data chunk; = 'data' (ASCII)

    unsigned char dataChunkLen[4];        // length of data
};

/*
 * A basic WAV output file type
 * Initial implementation by Michael Schwendt <mschwendt@yahoo.com>
 */
class WavFile: public AudioBase
{
private:
    unsigned long int byteCount;

    static const wavHeader defaultWavHdr;
    wavHeader wavHdr;

    std::ostream *file;
    bool headerWritten;  // whether final header has been written
    int precision;

public:

    WavFile();

    // Only unsigned 8-bit, and signed 16-bit, samples are supported.
    // Endian-ess is adjusted if necessary.
    //
    // If number of sample bytes is given, this can speed up the
    // process of closing a huge file on slow storage media.

    short *open(AudioConfig &cfg, const char *name);

    // After write call old buffer is invalid and you should
    // use the new buffer provided instead.
    short *write();
    void  close();
    void  pause() {;}
    const char *extension () const { return ".wav"; }
    ~WavFile() { close(); }

    // Rev 1.3 (saw) - Changed, see AudioBase.h
    short *reset ()
    {
        if (file != 0)
            return _sampleBuffer;
        return NULL;
    }

    // Stream state.
    bool fail() const { return (file->fail() != 0); }
    bool bad()  const { return (file->bad()  != 0); }

    operator bool()  const { return (file->good() != 0); }
    bool operator!() const { return (file->fail() != 0); }
};

#endif /* WAVE_FILE_HEADER_H */
