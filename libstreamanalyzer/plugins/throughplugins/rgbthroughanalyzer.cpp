/* This file is part of Strigi Desktop Search, ported from code of:
 * Copyright (C) 2004 Melchior FRANZ <mfranz@kde.org>
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
#include <strigi/textutils.h>
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>
#include <map>

using namespace std;
using namespace Strigi;


// AnalyzerFactory

// The SGI Image File Format
// http://local.wasp.uwa.edu.au/~pbourke/dataformats/sgirgb/sgiversion.html

void
RgbThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    bitDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    imageNameField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
    sharedRowsField = reg.registerField(
        "http://strigi.sf.net/ontologies/homeless#documentImageSharedRows");
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
    if (!in)
        return in;

    const char *c;
    uint16_t magic;
    uint8_t  storage;
    uint8_t  bpc;
    uint16_t dimension;
    uint16_t xsize;
    uint16_t ysize;
    uint16_t zsize;
    int32_t colormap;

    //Remember: dds files are big-endian
    //read the beginning of the stream and make sure it looks ok
    const int64_t pos = in->position();
    in->reset(pos);   // rewind to the start of the stream
    if (512 != in->read(c, 512, 512)) {
       in->reset(pos);
       return in;
    }
    in->reset(pos);
    magic = readBigEndianUInt16(c);
    if (magic != 474) {
        return in;
    }
    storage = (uint8_t)*(c+2);
    bpc = (uint8_t)*(c+3);
    dimension = readBigEndianUInt16(c+4);
    xsize = readBigEndianUInt16(c+6);
    ysize = readBigEndianUInt16(c+8);
    zsize = readBigEndianUInt16(c+10);
    //pixmin = readBigEndianUInt32(c+12);
    //pixmax = readBigEndianUInt32(c+16);
    //dummy = readBigEndianUInt32(c+20);
    colormap = readBigEndianUInt32(c+104);
    // check ranges
    if ((storage != 1 && storage != 0) || (bpc != 1 && bpc != 2)
            || dimension < 1 || dimension > 3
            || colormap < 0 || colormap > 3) {
        // not a valid RGB file
        return in;
    }
    string imagename(c+24, 79); // should be 79 ascii characters

    if (dimension == 1)
        ysize = 1;

    // report analysis
    analysisResult->addValue( factory->widthField, xsize );
    analysisResult->addValue( factory->heightField, ysize );
    analysisResult->addValue( factory->bitDepthField, zsize * 8 * bpc );
    if (imagename[0] != '\0') {
        analysisResult->addValue( factory->imageNameField, imagename);
    }
    /* //FIXME: either get rid of this or replace with NIE equivalent
    if (zsize == 1)
        analysisResult->addValue( factory->colorModeField, "Grayscale");
    else if (zsize == 2)
        analysisResult->addValue( factory->colorModeField, "Grayscale/Alpha");
    else if (zsize == 3)
        analysisResult->addValue( factory->colorModeField, "RGB");
    else if (zsize == 4)
        analysisResult->addValue( factory->colorModeField, "RGB/Alpha");

    if (storage == 0) {
        analysisResult->addValue( factory->compressionField, "Uncompressed");
    } else if (storage == 1) {
        analysisResult->addValue( factory->compressionField,
                                  string ("Runlength Encoded"));
    }
    */
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
