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

#include "jstreamsconfig.h"
#include "xpmthroughanalyzer.h"
#include "analysisresult.h"
#include "fieldtypes.h"

using namespace std;
using namespace jstreams;
using namespace Strigi;

// AnalyzerFactory
const cnstr XpmThroughAnalyzerFactory::typeFieldName("type");
const cnstr XpmThroughAnalyzerFactory::widthFieldName("width");
const cnstr XpmThroughAnalyzerFactory::heightFieldName("height");
const cnstr XpmThroughAnalyzerFactory::numberOfColorsFieldName("numberofcolors");

void
XpmThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.registerField(typeFieldName,
        FieldRegister::stringType, 1, 0);
    widthField = reg.registerField(widthFieldName,
        FieldRegister::integerType, 1, 0);
    heightField = reg.registerField(heightFieldName,
        FieldRegister::integerType, 1, 0);
    numberOfColorsField = reg.registerField(numberOfColorsFieldName,
        FieldRegister::integerType, 1, 0);
}

// Analyzer
void
XpmThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
XpmThroughAnalyzer::connectInputStream(InputStream* in) {
    const int32_t nreq = 9;
    const char* buf;

    int32_t nread = in->read(buf, nreq, -1);
    in->reset(0);

    if (nread < nreq || strncmp(buf, "/* XPM */", 9)) {
        return in;
    }

    int32_t i = 0;
    while (i < nread) {
        if (buf[i] == '"') {
            // read the height
            uint32_t propertyValue = 0;
            i++;
            while (i < nread && isdigit(buf[i])) {
                propertyValue = (propertyValue * 10) + buf[i] - '0';
                i++;
            }

            if (i >= nread || buf[i] != ' ')
                return in;

            analysisResult->setField(factory->heightField, propertyValue);

            // read the width
            propertyValue = 0;
            i++;
            while (i < nread && isdigit(buf[i])) {
                propertyValue = (propertyValue * 10) + buf[i] - '0';
                i++;
            }

            if (i >= nread || buf[i] != ' ')
                return in;

            analysisResult->setField(factory->widthField, propertyValue);

            // read the number of colors
            propertyValue = 0;
            i++;
            while (i < nread && isdigit(buf[i])) {
                propertyValue = (propertyValue * 10) + buf[i] - '0';
                i++;
            }

            if (i >= nread || buf[i] != ' ')
                return in;

            analysisResult->setField(factory->numberOfColorsField, propertyValue);
            break;
        }
        i++;
    }

    return in;
}

bool
XpmThroughAnalyzer::isReadyWithStream() {
    return true;
}


//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    getStreamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new XpmThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
