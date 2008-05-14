/* This file is part of Strigi Desktop Search, ported from code of:
 * - Melchior FRANZ <mfranz@kde.org> (Copyright (C) 2004)
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

#include "rgbthroughanalyzer.h"
#include "textutils.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "fieldtypes.h"
#include <cstring>
#include <map>
#include <config.h>

using namespace std;
using namespace Strigi;


// AnalyzerFactory

void
RgbThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#width");
    heightField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#height");
    bitDepthField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#pixelDataBitDepth");
    imageNameField = reg.registerField(
        "document.stats.image_name");
    sharedRowsField = reg.registerField(
        "document.stats.image_shared_rows");
    colorModeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
}

// Analyzer
void
RgbThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
RgbThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    const char *c;
    uint16_t magic;
    uint8_t  storage;
    uint8_t  bpc;
    uint16_t dimension;
    uint16_t xsize;
    uint16_t ysize;
    uint16_t zsize;
    uint32_t pixmin;
    uint32_t pixmax;
    uint32_t dummy;
    uint32_t colormap;

    //Remember: dds files are big-endian
    //read the beginning of the stream and make sure it looks ok
    
    if (2 != in->read(c, 2, 2)) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    magic = readBigEndianUInt16(c);

    if (magic != 474) {
        in->reset(0);
        return in;
    }

    if (1 != in->read(c, 1, 1)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    storage = (uint8_t)*c;

    if (1 != in->read(c, 1, 1)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    bpc = (uint8_t)*c;

    if (8 != in->read(c, 8, 8)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    dimension = readBigEndianUInt16(c);
    xsize = readBigEndianUInt16(c+2);
    ysize = readBigEndianUInt16(c+4);
    zsize = readBigEndianUInt16(c+6);

    if (12 != in->read(c, 12, 12)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    pixmin = readBigEndianUInt32(c);
    pixmax = readBigEndianUInt32(c+4);
    dummy = readBigEndianUInt32(c+8);

    if (80 != in->read(c, 80, 80)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    string imagename (c, 80);
    //TODO: discover why imagename is reported as a NON utf8 string
    imagename[79] = '\0';

    if (4 != in->read(c, 4, 4)) {
        in->reset(0);   // rewind to the start of the stream
        return in;
    }
    colormap = readBigEndianUInt32(c);

    if (404 != in->skip( 404)) {
        in->reset (0);
        return in;
    }

    if (dimension == 1)
        ysize = 1;

    //determine fileSize
    uint64_t fileSize;
    uint64_t pos;
    pos = in->position();
    fileSize = pos;
    uint64_t skipped;
    do{
        skipped = in->skip( 8192);
        fileSize += skipped;
    } while (skipped != 0);

    // restore file position
    in->reset (pos);
    
    analysisResult->addValue( factory->widthField, xsize );
    analysisResult->addValue( factory->heightField, ysize );
    analysisResult->addValue( factory->bitDepthField, zsize * 8 * bpc );
    analysisResult->addValue( factory->imageNameField, imagename);

    if (zsize == 1)
        analysisResult->addValue( factory->colorModeField, "Grayscale");
    else if (zsize == 2)
        analysisResult->addValue( factory->colorModeField, "Grayscale/Alpha");
    else if (zsize == 3)
        analysisResult->addValue( factory->colorModeField, "RGB");
    else if (zsize == 4)
        analysisResult->addValue( factory->colorModeField, "RGB/Alpha");

    if (!storage)
        analysisResult->addValue( factory->compressionField, "Uncompressed");
    else if (storage == 1) {
        uint64_t compressed = fileSize - 512;
        long verbatim = xsize * ysize * zsize;
        char buff[50];
        snprintf (buff, 50, "%.1f", compressed * 100.0 / verbatim);
        analysisResult->addValue( factory->compressionField,
                                  string ("Runlength Encoded, ") + buff);

        long k;
        uint32_t offs;
        map<uint32_t, uint> pixelMap;
        map<uint32_t, uint>::iterator it;
        map<uint32_t, uint>::iterator end = pixelMap.end();
        for (k = 0; k < (ysize * zsize); k++) {
            
            if (4 != in->read(c, 4, 4)) {
                in->reset(0);   // rewind to the start of the stream
                return in;
            }
            offs = readBigEndianUInt32 (c);
            
            if ((it = pixelMap.find(offs)) != end)
                it->second = it->second + 1;
            else
                pixelMap[offs] = 0;
        }

        for (k = 0, it = pixelMap.begin(); it != end; ++it){
            k += it->second;
        }
        if (k) {
            snprintf (buff, 50, "%.1f", k * 100.0 / (ysize * zsize));
            analysisResult->addValue( factory->sharedRowsField, buff);
        }
        else
            analysisResult->addValue( factory->sharedRowsField, "None");
    } else
        analysisResult->addValue( factory->compressionField, "Unknown");

    
    
    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
RgbThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new RgbThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
