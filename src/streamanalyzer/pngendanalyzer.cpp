/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "jstreamsconfig.h"
#include "pngendanalyzer.h"
#include "analysisresult.h"
#include "textendanalyzer.h"
#include "subinputstream.h"
#include "fieldtypes.h"
#include "gzipinputstream.h"
#include "textutils.h"
using namespace std;
using namespace jstreams;
using namespace Strigi;

const cnstr PngEndAnalyzerFactory::widthFieldName("width");
const cnstr PngEndAnalyzerFactory::heightFieldName("height");
const cnstr PngEndAnalyzerFactory::colorDepthFieldName("colorDepth");
const cnstr PngEndAnalyzerFactory::colorModeFieldName("colorMode");
const cnstr PngEndAnalyzerFactory::compressionFieldName("compression");
const cnstr PngEndAnalyzerFactory::interlaceModeFieldName("interlaceMode");

// and for the colors
static const char* colors[] = {
  "Grayscale",
  "Unknown",
  "RGB",
  "Palette",
  "Grayscale/Alpha",
  "Unknown",
  "RGB/Alpha"
};

static const char* interlaceModes[] = {
  "None",
  "Adam7"
};

void
PngEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField
        = reg.registerField(widthFieldName, FieldRegister::integerType, 1, 0);
    heightField = reg.registerField(heightFieldName,
        FieldRegister::integerType, 1, 0);
    colorDepthField = reg.registerField(colorDepthFieldName,
        FieldRegister::integerType, 1, 0);
    colorModeField = reg.registerField(colorModeFieldName,
        FieldRegister::integerType, 1, 0);
    compressionField = reg.registerField(compressionFieldName,
        FieldRegister::integerType, 1, 0);
    interlaceModeField = reg.registerField(interlaceModeFieldName,
        FieldRegister::integerType, 1, 0);
}

bool
PngEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char pngmagic[]
        = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    return headersize >= 29 &&  memcmp(header, pngmagic, 8) == 0;
}
char
PngEndAnalyzer::analyze(AnalysisResult& rs, InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 12, 12);
    if (nread != 12) {
        // file is too small to be a png
        return -1;
    }

    // read chunksize and include the size of the type and crc (4 + 4)
    uint32_t chunksize = readBigEndianUInt32(c+8) + 8;
    if (chunksize > 1048576) {
        fprintf(stderr,"chunk too big: %u\n",chunksize);
        return -1;
    }
    nread = in->read(c, chunksize, chunksize);
    // the IHDR chunk should be the first
    if (nread != (int32_t)chunksize || strncmp(c, "IHDR", 4)) {
        return -1;
    }

    // read the png dimensions
    uint32_t width = readBigEndianUInt32(c+4);
    uint32_t height = readBigEndianUInt32(c+8);
    rs.setField(factory->widthField, width);
    rs.setField(factory->heightField, height);

    uint16_t type = c[13];
    uint16_t bpp = c[12];

    // the bpp are only per channel, so we need to multiply the with
    // the channel count
    switch (type) {
        case 0: break;           // Grayscale
        case 2: bpp *= 3; break; // RGB
        case 3: break;           // palette
        case 4: bpp *= 2; break; // grayscale w. alpha
        case 6: bpp *= 4; break; // RGBA

        default: // we don't get any sensible value here
            bpp = 0;
    }

    rs.setField(factory->colorDepthField, bpp);
    rs.setField(factory->colorModeField,
        (type < sizeof(colors)/sizeof(colors[0]))
                   ? colors[type] : "Unknown");

    rs.setField(factory->compressionField,
        (c[14] == 0) ? "Deflate" : "Unknown");

    rs.setField(factory->interlaceModeField,
        ((uint)c[16] < sizeof(interlaceModes)/sizeof(interlaceModes[0]))
                   ? interlaceModes[(int)c[16]] : "Unknown");

    // read the rest of the chunks
    // TODO: check if we want a fast or complete analysis
    // read the type
    nread = in->read(c, 8, 8);
    while (nread == 8 && strncmp("IEND", c+4, 4)) {
        // get the size of the data block
        chunksize = readBigEndianUInt32(c);

        // TODO read http://tools.ietf.org/html/rfc2083 to handle
        // predefined text values and map them to good semantic fields

        // TODO actually extract text, right now we only skip it
        if (strncmp("tEXt", c+4, 4) == 0) {
            // TODO convert latin1 to utf8 and analyze the format properly
            SubInputStream sub(in, chunksize);
            analyzeText(rs, &sub);
            sub.skip(chunksize);
        } else if (strncmp("zTXt", c+4, 4) == 0) {
            SubInputStream sub(in, chunksize);
            analyzeZText(rs, &sub);
            sub.skip(chunksize);
        } else if (strncmp("iTXt", c+4, 4) == 0) {
            SubInputStream sub(in, chunksize);
            analyzeText(rs, &sub);
            sub.skip(chunksize);
        } else {
            nread = (int32_t)in->skip(chunksize);
            if (nread != (int32_t)chunksize) {
                fprintf(stderr, "could not skip chunk size %u\n", chunksize);
                return -1;
            }
        }
        in->skip(4); // skip crc
        nread = in->read(c, 8, 8);
    }
    if (nread != 8) {
        // invalid file or error
        fprintf(stderr, "bad end\n");
        return -1;
    }

    return 0;
}
char
PngEndAnalyzer::analyzeText(Strigi::AnalysisResult& as,
        jstreams::InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 80, 80);
    if (nread < 1) {
         return -1;
    }
    // find the \0
    int32_t nlen = 0;
    while (nlen < nread && c[nlen]) nlen++;
    if (nlen == nread) return -1;
    string name(c, nlen); // do something with the name!
    in->reset(nlen+1);
    TextEndAnalyzer tea;
    return tea.analyze(as, in);
}
char
PngEndAnalyzer::analyzeZText(Strigi::AnalysisResult& as,
        jstreams::InputStream* in) {
    const char* c;
    int32_t nread = in->read(c, 81, 81);
    if (nread < 1) {
         return -1;
    }
    // find the \0
    int32_t nlen = 0;
    while (nlen < nread && c[nlen]) nlen++;
    if (nlen == nread) return -1;
    string name(c, nlen); // do something with the name!
    in->reset(nlen+2);
    GZipInputStream z(in, GZipInputStream::ZLIBFORMAT);
    TextEndAnalyzer tea;
    return tea.analyze(as, &z);
}
