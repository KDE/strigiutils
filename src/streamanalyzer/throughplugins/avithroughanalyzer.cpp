/* This file is part of Strigi Desktop Search, ported from code of:
 * - Shane Wright <me@shanewright.co.uk> (Copyright (C) 2002)
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

#include "avithroughanalyzer.h"
#include "textutils.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "fieldtypes.h"
#include <cstring>

using namespace std;
using namespace Strigi;

// AnalyzerFactory

//TODO: check values!
const string AviThroughAnalyzerFactory::lengthFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.length" );
const string AviThroughAnalyzerFactory::resolutionHeightFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.resolutionHeight" );
const string AviThroughAnalyzerFactory::resolutionWidthFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.resolutionWidth" );
const string AviThroughAnalyzerFactory::frameRateFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.framerate" );
const string AviThroughAnalyzerFactory::videoCodecFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.videocodec" );
const string AviThroughAnalyzerFactory::audioCodecFieldName( "http://freedesktop.org/standards/xesam/1.0/core#video.audiocodec" );

void
AviThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    lengthField = reg.registerField(lengthFieldName);
    resolutionHeightField = reg.registerField(resolutionHeightFieldName);
    resolutionWidthField = reg.registerField(resolutionWidthFieldName);
    frameRateField = reg.registerField(frameRateFieldName);
    videoCodecField = reg.registerField(videoCodecFieldName);
    audioCodecField = reg.registerField(audioCodecFieldName);
}

// Analyzer
void
AviThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

bool
AviThroughAnalyzer::read_avi(InputStream* in)
{
    static const char sig_riff[] = "RIFF";
    static const char sig_avi[]  = "AVI ";
    static const char sig_list[] = "LIST";
    static const char sig_junk[] = "JUNK";
    uint32_t charbuf1;
    uint32_t dwbuf1;
    const char* c;

    done_avih = false;
    done_audio = false;

    // read AVI header
    // this must be RIFF
    if (4 != in->read ( c, 4, 4))
        return false;
    charbuf1 = readLittleEndianUInt32(c);
    if (memcmp(&charbuf1, sig_riff, 4) != 0)
        return false;

    if (4 != in->read ( c, 4, 4))
        return false;
    dwbuf1 = readLittleEndianUInt32(c);

    // this must be AVI
    if (4 != in->read ( c, 4, 4))
        return false;
    charbuf1 = readLittleEndianUInt32(c);
    if (memcmp(&charbuf1, sig_avi, 4) != 0)
        return false;

    // start reading AVI file
    int counter = 0;
    bool done = false;
    do {
        //printf ("read_avi, in position: %i\n", in->position());
        // read header
        if (4 != in->read ( c, 4, 4))
            return false;
        charbuf1 = readLittleEndianUInt32(c);

        //printf ("about to handle chunk with ID: %i\n", charbuf1);

        if (memcmp(&charbuf1, sig_list, 4) == 0) {
            // if list
            if (!read_list(in))
                return false;
        }
        else if (memcmp(&charbuf1, sig_junk, 4) == 0) {
            // if junk

            // read chunk size
            if (4 != in->read ( c, 4, 4))
                return false;
            dwbuf1 = readLittleEndianUInt32(c);

            //printf ("Skipping junk chunk length: %i\n", dwbuf1);

            // skip junk

            //TODO: why reset doesn't work?! we've to use skip
            //in->reset( in->position() + dwbuf1);
            in->skip (dwbuf1);
            //printf ("After skip: %i\n", in->position());
        }
        else {
            // something we don't understand yet
            //printf ("Unknown chunk header found: %i", charbuf1);
            return false;
        }

        uint64_t curr = in->position();
        bool atEnd = false;
        if (1 != in->read( c, 1, 1))
            atEnd = true;
        in->reset ( curr);
        
        if (((done_avih) && (strlen(handler_vids) > 0)
              && (done_audio)) || atEnd) {
            //printf ("We're done!\n");
            done = true;
        }

        // make sure we don't stay here forever
        ++counter;
        if (counter > 10)
            done = true;

    } while (!done);

    return true;
}

bool
AviThroughAnalyzer::read_list(InputStream* in)
{
    //printf ("read_list, in position: %i\n", in->position());
    const char sig_hdrl[] = "hdrl";   // header list
    const char sig_strl[] = "strl";   // ...list
    const char sig_movi[] = "movi";   // movie list

    const char* c;
    uint32_t dwbuf1;
    uint32_t charbuf1;

    //printf ("In read_list()\n");

    // read size & list type
    if (8 != in->read ( c, 8, 8))
        return false;
    dwbuf1 = readLittleEndianUInt32(c);
    charbuf1 = readLittleEndianUInt32(c+4);

    // read the relevant bits of the list
    if (memcmp(&charbuf1, sig_hdrl, 4) == 0) {
        // should be the main AVI header
        if (!read_avih(in))
            return false;
    } else if (memcmp(&charbuf1, sig_strl, 4) == 0) {
        // should be some stream info
        if (!read_strl(in))
            return false;
    } else if (memcmp(&charbuf1, sig_movi, 4) == 0) {
        // movie list
        //printf ("Skipping movi chunk length: %i\n", dwbuf1);

        // skip past it
        in->reset (in->position() + dwbuf1);
    } else {
        // unknown list type
        //printf ("Unknown list type found: %i\n", charbuf1);
    }

    return true;
}

bool
AviThroughAnalyzer::read_avih(InputStream* in)
{
    //printf ("read_avih, in position: %i\n", in->position());
    static const char sig_avih[] = "avih";   // header list

    uint32_t dwbuf1;
    uint32_t charbuf1;
    const char* c;

    // read header and length
    if (8 != in->read ( c, 8, 8))
        return false;
    charbuf1 = readLittleEndianUInt32(c);
    dwbuf1 = readLittleEndianUInt32(c+4);

    // not a valid avih?
    if (memcmp(&charbuf1, sig_avih, 4) != 0) {
        //printf ("Chunk ID error, expected avih, got: %i\n", charbuf1);
        return false;
    }

    // read all the avih fields
    if (56 != in->read ( c, 56, 56))
        return false;

    avih_microsecperframe = readLittleEndianUInt32(c);
    avih_maxbytespersec  = readLittleEndianUInt32(c+4);
    avih_reserved1 = readLittleEndianUInt32(c+8);
    avih_flags = readLittleEndianUInt32(c+12);
    avih_totalframes = readLittleEndianUInt32(c+16);
    avih_initialframes = readLittleEndianUInt32(c+20);
    avih_streams = readLittleEndianUInt32(c+24);
    avih_buffersize = readLittleEndianUInt32(c+28);
    avih_width = readLittleEndianUInt32(c+32);
    avih_height = readLittleEndianUInt32(c+36);
    avih_scale = readLittleEndianUInt32(c+40);
    avih_rate = readLittleEndianUInt32(c+44);
    avih_start = readLittleEndianUInt32(c+48);
    avih_length = readLittleEndianUInt32(c+52);

    done_avih = true;

    return true;
}

bool
AviThroughAnalyzer::read_strl(InputStream* in)
{
    static const char sig_strh[] = "strh";
    static const char sig_strf[] = "strf";
        //static const char sig_strd[] = "strd";
    static const char sig_strn[] = "strn";
    static const char sig_list[] = "LIST";
    static const char sig_junk[] = "JUNK";

    //printf ("in strl handler\n");

    uint32_t dwbuf1;    // buffer for block sizes
    uint32_t charbuf1;
    const char* c;

        // loop through blocks
    int counter = 0;
    while (true) {
        //printf ("read_strl, while start in position: %i\n", in->position());
        // read type and size
        if (8 != in->read ( c, 8, 8))
            return false;
        charbuf1 = readLittleEndianUInt32(c); // type
        dwbuf1 = readLittleEndianUInt32(c+4); // size
        //printf ("read_strl, dwbuf1 while initial value = %i\n",dwbuf1);

        // detect type
        if (memcmp(&charbuf1, sig_strh, 4) == 0) {
            // got strh - stream header
            //printf ("Found strh, calling read_strh()\n");
            read_strh(in, dwbuf1);
        }
        else if (memcmp(&charbuf1, sig_strf, 4) == 0) {
            // got strf - stream format
            //printf ("Found strf, calling read_strf()\n");
            read_strf(in, dwbuf1);
        }
        else if (memcmp(&charbuf1, sig_strn, 4) == 0) {
            // we ignore strn, but it can be recorded incorrectly so we have
            // to cope especially

            // skip it
            //printf ("Skipping strn chunk length: %i\n", dwbuf1);
            in->reset (in->position() + dwbuf1);

            /*
            this is a pretty annoying hack; many AVIs incorrectly report the
            length of the strn field by 1 byte.  Its possible that strn's
            should be word aligned, but no mention in the specs...

            I'll clean/optimise this a touch soon
            */

            bool done = false;
            unsigned char counter = 0;
            while (!done) {
                // read next marker
                if (4 != in->read ( c, 4, 4))
                    return false;
                charbuf1 = readLittleEndianUInt32(c); // type

                // does it look ok?
                if ((memcmp(&charbuf1, sig_list, 4) == 0) ||
                     (memcmp(&charbuf1, sig_junk, 4) == 0)) {
                        // yes, go back before it
                        in->reset(in->position() - 4);
                        done = true;
                     } else {
                        // no, skip one space forward from where we were
                         in->reset (in->position() - 3);
                         //printf ("Working around incorrectly marked strn length...\n");
                     }

                    // make sure we don't stay here too long
                     ++counter;
                     if (counter>10)
                         done = true;
            }
        }
        else if ((memcmp(&charbuf1, sig_list, 4) == 0) ||
                 (memcmp(&charbuf1, sig_junk, 4) == 0)) {
            // we have come to the end of our stay here in strl, time to leave
            //printf ("Found LIST/JUNK, returning...\n");
            //printf ("Found LIST/JUNK, returning from pos %i ...\n", in->position());
            // rollback before the id and size
            in->reset(in->position() - 8);

            // return back to the main avi parser
            return true;
        }
        else {
            // we have some other unrecognised block type
            //printf ("Skipping unrecognised block using dwbuf1 = %i\n", dwbuf1);
            // just skip over it
            in->reset (in->position() + dwbuf1);
        } /* switch block type */

        ++counter;
        if (counter > 10)
            return true;

    } /* while (true) */

    // we should never get here
}

bool
AviThroughAnalyzer::read_strh(InputStream* in, uint32_t blocksize)
{
    //printf ("read_strh, in position: %i\n", in->position());
    static const char sig_vids[] = "vids";   // ...video
    static const char sig_auds[] = "auds";   // ...audio

    uint32_t strh_flags;
    uint32_t strh_reserved1;
    uint32_t strh_initialframes;
    uint32_t strh_scale;
    uint32_t strh_rate;
    uint32_t strh_start;
    uint32_t strh_length;
    uint32_t strh_buffersize;
    uint32_t strh_quality;
    uint32_t strh_samplesize;

    uint32_t charbuf1;
    uint32_t charbuf2;
    const char* c;

    // get stream info type, and handler id
    if (8 != in->read ( c, 8, 8))
        return false;
    charbuf1 = readLittleEndianUInt32(c);
    charbuf2 = readLittleEndianUInt32(c+4);

    // read the strh fields
    if (40 != in->read ( c, 40, 40))
        return false;
        
    strh_flags = readLittleEndianUInt32(c);
    strh_reserved1 = readLittleEndianUInt32(c+4);
    strh_initialframes = readLittleEndianUInt32(c+8);
    strh_scale = readLittleEndianUInt32(c+12);
    strh_rate = readLittleEndianUInt32(c+16);
    strh_start = readLittleEndianUInt32(c+20);
    strh_length = readLittleEndianUInt32(c+24);
    strh_buffersize = readLittleEndianUInt32(c+28);
    strh_quality = readLittleEndianUInt32(c+32);
    strh_samplesize = readLittleEndianUInt32(c+36);

    if (memcmp(&charbuf1, sig_vids, 4) == 0) { // we are video!
        // save the handler
        memcpy(handler_vids, &charbuf2, 4);
        //printf ("Video handler: %s\n", handler_vids);
    }
    else if (memcmp(&charbuf1, sig_auds, 4) == 0) { // we are audio!
        // save the handler
        memcpy(handler_auds, &charbuf2, 4);
        //printf ("Audio handler: %X\n", handler_auds);

        // we want strf to get the audio codec
        wantstrf = true;
    } else {
        // we are something that we don't understand
    }

    // do we need to skip ahead any more?  (usually yes , contrary to
    // the AVI specs I've read...)
    // note: 48 is 10 * uint32_t + 2*FOURCC; the 10 fields we read above, plus the two character fields
    //printf ("read_strh, before end position: %i\n", in->position());
    if (blocksize > 48) {
        in->reset (in->position() + (blocksize - 48));
        //printf ("read_strh, new position after reset: %i\n", in->position());
    }

    return true;
}

bool
AviThroughAnalyzer::read_strf(InputStream* in, uint32_t blocksize)
{
    const char* c;
    //printf ("read_strf, in position: %i\n", in->position());
    
    // do we want to do the strf?
    if (wantstrf) {
        // yes.  we want the audio codec identifier out of it

        // get the 16bit audio codec ID
        if (2 != in->read ( c, 2, 2))
            return false;
        handler_audio = readLittleEndianUInt16(c);
            
        //printf ("Read audio codec ID: %X\n", handler_audio);
        // skip past the rest of the stuff here for now
        in->reset( in->position() + blocksize - 2);
        // we have audio
        done_audio = true;

    } else {
        //printf ("read_strf: skipping %i\n", blocksize);
        // no, skip the strf
        in->reset (in->position() + blocksize);
    }

    return true;
}

const char*
AviThroughAnalyzer::resolve_audio(uint16_t id)
{
    /*
    this really wants to use some sort of KDE global
    list.  To avoid bloat for the moment it only does
    a few common codecs
    */

    static const char codec_unknown[] = "Unknown";
    static const char codec_01[]  = "Microsoft PCM";
    static const char codec_02[]  = "Microsoft ADPCM";
    static const char codec_50[]  = "MPEG";
    static const char codec_55[]  = "MP3";
    static const char codec_92[]  = "AC3";
    static const char codec_160[] = "WMA1";
    static const char codec_161[] = "WMA2";
    static const char codec_162[] = "WMA3";
    static const char codec_2000[] = "DVM";
    switch (id) {
        case 0x000 : return codec_unknown; break;
        case 0x001 : return codec_01; break;
        case 0x002 : return codec_02; break;
        case 0x050 : return codec_50; break;
        case 0x055 : return codec_55; break;
        case 0x092 : return codec_92; break;
        case 0x160 : return codec_160; break;
        case 0x161 : return codec_161; break;
        case 0x162 : return codec_162; break;
        case 0x2000 : return codec_2000; break;
        default : return codec_unknown;
    }

    return NULL;
}

InputStream*
AviThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    /***************************************************/
    // prep

    memset(handler_vids, 0x00, 5);
    memset(handler_auds, 0x00, 5);

    /***************************************************/
    // start reading stuff from it
    wantstrf = false;

    if (!read_avi(in)) {
        //printf ("read_avi() failed!\n");
    }

    /***************************************************/
    // set up our output

    if (done_avih) {

        if (0 != avih_microsecperframe)
            analysisResult->addValue( factory->frameRateField,
                                      int (1000000 / avih_microsecperframe ));
        
        analysisResult->addValue( factory->resolutionHeightField, avih_width);
        analysisResult->addValue( factory->resolutionHeightField, avih_height);

        // work out and add length
        uint64_t mylength = (uint64_t) ((float) avih_totalframes * (float) avih_microsecperframe / 1000000.0);
        analysisResult->addValue( factory->lengthField, int(mylength));

        if (strlen(handler_vids) > 0)
            analysisResult->addValue( factory->videoCodecField, handler_vids);
        else
            analysisResult->addValue( factory->videoCodecField, "Unknown");

        if (done_audio)
            analysisResult->addValue( factory->audioCodecField,
                                      resolve_audio(handler_audio));
        else
            analysisResult->addValue( factory->audioCodecField, "None");
    }

    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
AviThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new AviThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
