/* This file is part of Strigi Desktop Search, ported from code of:
 * Copyright (C) 2007 Shane Wright <me@shanewright.co.uk>
 *                    Matthias Lechner <matthias@lmme.de>
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

#include "icothroughanalyzer.h"
#include <strigi/textutils.h>
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>

using namespace std;
using namespace Strigi;

// AnalyzerFactory

void
IcoThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    numberField = reg.registerField(
        "http://strigi.sf.net/ontologies/homeless#documentImageCount");
    bitsPerPixelField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    colorCountField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorCount");

    addField(widthField);
    addField(heightField);
    addField(numberField);
    addField(bitsPerPixelField);
    addField(colorCountField);
}

// Analyzer
void
IcoThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
IcoThroughAnalyzer::connectInputStream(InputStream* in) {
    if( !in )
        return in;

    const char *c;
    int32_t n;

    // Remember: ICO files are little-endian
    // read the beginning of the stream and make sure it looks ok

    n = in->read(c, 6, 6);
    if (n != 6) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint16_t ico_reserved = readLittleEndianUInt16(c);
    uint16_t ico_type = readLittleEndianUInt16(c+2);
    uint16_t ico_count = readLittleEndianUInt16(c+4);

    if ((ico_reserved != 0) || (ico_type != 1) || (ico_count < 1)) {
        in->reset(0);
        return in;
    }

    // now loop through each of the icon entries
    n = in->read(c, 1, 1);
    if (n != 1) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint8_t icoe_width = (uint8_t)*c;

    n = in->read(c, 1, 1);
    if (n != 1) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint8_t icoe_height = (uint8_t)*c;

    n = in->read(c, 1, 1);
    if (n != 1) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint8_t icoe_colorcount = (uint8_t)*c;

    n = in->read(c, 1, 1);
    if (n != 1) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    //uint8_t icoe_reserved = (uint8_t)*c;

    n = in->read(c, 2, 2);
    if (n != 2) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    /*uint16_t icoe_planes =*/ readLittleEndianUInt16(c);

    n = in->read(c, 2, 2);
    if (n != 2) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    uint16_t icoe_bitcount = readLittleEndianUInt16(c);

    n = in->read(c, 4, 4);
    if (n != 4) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    /*uint32_t icoe_bytesinres =*/ readLittleEndianUInt32(c);

    n = in->read(c, 4, 4);
    if (n != 4) {
       in->reset(0);   // rewind to the start of the stream
       return in;
    }
    /*uint32_t icoe_imageoffset =*/ readLittleEndianUInt32(c);

    // read the data on the 1st icon
    //FIXME: either get rid of this or replace with NIE equivalent
    //analysisResult->addValue( factory->numberField, ico_count );

    analysisResult->addValue( factory->widthField, icoe_width );
    analysisResult->addValue( factory->heightField, icoe_height );

    if (icoe_bitcount > 0)
        analysisResult->addValue( factory->bitsPerPixelField, icoe_bitcount );

    if (icoe_colorcount > 0)
        analysisResult->addValue( factory->colorCountField, icoe_colorcount );
    else if (icoe_bitcount > 0)
        analysisResult->addValue( factory->colorCountField, 2 ^ icoe_bitcount );

    in->reset(0);   // rewind to the start of the stream
    return in;
}

bool
IcoThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new IcoThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
