/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

#include "strigiconfig.h"
#include "analysisresult.h"
#include "fieldtypes.h"
#include "textutils.h"
#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"

#include <string.h>

// this version does not extract comment at the moment
// read http://www.w3.org/Graphics/GIF/spec-gif89a.txt on how to do that

using namespace std;
using namespace Strigi;

namespace Strigi {
    class RegisteredField;
}
class GifThroughAnalyzerFactory;

class STRIGI_PLUGIN_API GifThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const GifThroughAnalyzerFactory* factory;
public:
    GifThroughAnalyzer(const GifThroughAnalyzerFactory* f) :factory(f) {}
    ~GifThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
};

class GifThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class GifThroughAnalyzer;
private:
    const Strigi::RegisteredField* colorDepthField;
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const char* name() const {
        return "GifThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new GifThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

// AnalyzerFactory

void
GifThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    colorDepthField = reg.registerField("image.color_depth",
        FieldRegister::integerType, 1, 0);
    widthField = reg.registerField("image.width",
        FieldRegister::integerType, 1, 0);
    heightField = reg.registerField("image.height",
        FieldRegister::integerType, 1, 0);
}

// Analyzer
void
GifThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
GifThroughAnalyzer::connectInputStream(InputStream* in) {
    const char* buf;
    int32_t nread = in->read(buf, 12, -1);
    in->reset(0);

    if (nread < 12 || strncmp(buf, "GIF8", 4) || buf[5] != 'a'
            || (buf[4] != '7' && buf[4] != '9')) {
        return in;
    }

    uint16_t width = readLittleEndianUInt16(buf+6);
    uint16_t height = readLittleEndianUInt16(buf+8);
    analysisResult->addValue(factory->widthField, width);
    analysisResult->addValue(factory->heightField, height);

    if (buf[4] == 7) {
        uint8_t colorDepth = (buf[9] & 0x07) + 1;
        analysisResult->addValue(factory->colorDepthField, colorDepth);
    }

    return in;
}

bool
GifThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new GifThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
