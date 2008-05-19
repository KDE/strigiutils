/* This file is part of Strigi Desktop Search, ported from code of:
 * Copyright (C) 2002 Ryan Cumming <bodnar42@phalynx.dhs.org>
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "wavthroughanalyzer.h"
#include "textutils.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "fieldtypes.h"
#include <cstring>

using namespace std;
using namespace Strigi;

// AnalyzerFactory

void
WavThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    sampleSizeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#audioSampleBitDepth");
    sampleRateField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#audioSampleRate");
    channelsField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#audioChannels");
    lengthField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#mediaDuration");
    addField(sampleSizeField);
    addField(sampleRateField);
    addField(channelsField);
    addField(lengthField);
}

// Analyzer
void
WavThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
WavThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    const char *c;
    uint32_t format_size;
    uint16_t format_tag;
    uint16_t channel_count = 0;
    uint32_t sample_rate = 0;
    uint32_t bytes_per_second = 0;
    uint16_t bytes_per_sample;
    uint16_t sample_size = 0;
    uint32_t data_size = 0;
    uint32_t unknown_chunk_size;
    bool have_fmt = false;
    bool have_data = false;
    bool eof = false;

    static const char riff_signature[] = "RIFF";
    static const char wav_signature[] = "WAVE";
    static const char fmt_signature[] = "fmt ";
    static const char data_signature[] = "data";
    uint32_t signature_buffer;

    // Remember: WAV files are little-endian
    // Read and verify the RIFF signature
    if (4 != in->read(c, 4, 4)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    signature_buffer = readLittleEndianUInt32(c);

    if (memcmp(&signature_buffer, riff_signature, 4)) {
        in->reset(0);
        return in;
    }

    // Skip the next bit (total file size, pretty useless)
    if (4 != in->skip (4)) {
        in->reset(0);
        return in;
    }

    // Read and verify the WAVE signature
    if (4 != in->read(c, 4, 4)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    signature_buffer = readLittleEndianUInt32(c);

    if (memcmp(&signature_buffer, wav_signature, 4)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }

    // pretty dumb scanner, but better than what we had!
    channel_count = 0;
    do {
        if (4 != in->read(c, 4, 4)) {
            in->reset(0);   // rewind to the start of the stream
            return in;
        }
        signature_buffer = readLittleEndianUInt32(c);
        
        if (!memcmp(&signature_buffer, fmt_signature, 4)) {
            if (20 != in->read(c, 20, 20)) {
                in->reset(0);   // rewind to the start of the stream
                return in;
            }

            format_size = readLittleEndianUInt32 (c);
            format_tag = readLittleEndianUInt16 (c+4);
            channel_count = readLittleEndianUInt16 (c+6);
            sample_rate = readLittleEndianUInt32 (c+8);
            bytes_per_second = readLittleEndianUInt32 (c+12);
            bytes_per_sample = readLittleEndianUInt16 (c+16);
            sample_size = readLittleEndianUInt16 (c+18);
            have_fmt = true;
            if ( format_size > 16 ) {
                for (unsigned int i = 0; i < (format_size-16+1)/2; i++) {
                    if (2 != in->skip(2)) {
                        in->reset (0);
                        return in;
                    }
                }
            }
        }
        else if (!memcmp(&signature_buffer, data_signature, 4)) {
            if (4 != in->read(c, 4, 4)) {
                in->reset(0);   // rewind to the start of the stream
                return in;
            }
            data_size = readLittleEndianUInt32 (c);
            have_data = true;
        }
        else {
            if (4 != in->read(c, 4, 4)) {
                in->reset(0);   // rewind to the start of the stream
                return in;
            }
            unknown_chunk_size = readLittleEndianUInt32 (c);
            for (unsigned int i = 0; i < (unknown_chunk_size+1)/2; i++) {
                if (in->skip (2)) {
                    in->reset(0);   // rewind to the start of the stream
                    return in;
                }
            }
        }
        if (have_data && have_fmt)
            break;

        if (1 != in->read(c, 1, 1))
            eof = true;
        else
            in->reset(in->position() - 1);
        
    } while (!eof);

    if ( (!have_data) || (!have_fmt) ) {
        in->reset (0);
        return in;
    }

    // These values are downright illegal
    if ((!channel_count) || (!bytes_per_second)) {
        in->reset (0);
        return in;
    }

    analysisResult->addValue( factory->sampleSizeField, sample_size );
    analysisResult->addValue( factory->sampleRateField, sample_rate);
    analysisResult->addValue( factory->channelsField, channel_count);
    unsigned int wav_seconds = data_size / bytes_per_second;
    analysisResult->addValue( factory->lengthField, wav_seconds);

    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
WavThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new WavThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
