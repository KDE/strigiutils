/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Vincent Ricard <magic@magicninja.org>
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

#include "tgathroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>

using namespace std;
using namespace Strigi;

// AnalyzerFactory

void
TgaThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
    colorDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    colorModeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#colorSpace");
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    typeField = reg.typeField;

    addField(compressionField);
    addField(colorDepthField);
    addField(colorModeField);
    addField(widthField);
    addField(heightField);
    addField(typeField);
}

// Analyzer
void
TgaThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
TgaThroughAnalyzer::connectInputStream(InputStream* in) {
    if (in == 0) return 0;

    const int32_t nreq = 18;
    const char* buf;
    int32_t nread = in->read(buf, nreq, -1);
    in->reset(0);

    if (nread < nreq) {
        return in;
    }

    uint8_t imagetype = *(buf+2);
    /* //FIXME: either get rid of this or replace with NIE equivalent
    switch (imagetype) {
    case 1 :
    case 9 :
    case 32 :
        analysisResult->addValue(factory->colorModeField, "Color-Mapped");
        break;
    case 2 :
    case 10 :
    case 33 :
        analysisResult->addValue(factory->colorModeField, "RGB");
        break;
    case 3 :
    case 11 :
        analysisResult->addValue(factory->colorModeField, "Black and White");
        break;
    default :
        analysisResult->addValue(factory->colorModeField, "Unknown");
    }

    switch (imagetype) {
    case 1 :
    case 2 :
    case 3 :
        analysisResult->addValue(factory->compressionField, "None");
        break;
    case 9 :
    case 10 :
    case 11 :
        analysisResult->addValue(factory->compressionField, "RLE");
        break;
    case 32 :
        analysisResult->addValue(factory->compressionField, "Huffman, Delta & RLE");
        break;
    case 33 :
        analysisResult->addValue(factory->compressionField, "Huffman, Delta, RLE (4-pass quadtree)");
        break;
    default :
        analysisResult->addValue(factory->compressionField, "Unknown");
    };
    */
    uint16_t width = readLittleEndianUInt16(buf+12);
    uint16_t height = readLittleEndianUInt16(buf+14);
    analysisResult->addValue(factory->widthField, width);
    analysisResult->addValue(factory->heightField, height);

    uint8_t colorDepth = *(buf+16);
    analysisResult->addValue(factory->colorDepthField, colorDepth);
    analysisResult->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");

    return in;
}

bool
TgaThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new TgaThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory);
