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

#include "xpmlineanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>

using namespace std;
using namespace Strigi;

// AnalyzerFactory
void
XpmLineAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    numberOfColorsField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorCount");
    typeField = reg.typeField;

    addField(widthField);
    addField(heightField);
    addField(numberOfColorsField);
    addField(typeField);
}

// Analyzer
void
XpmLineAnalyzer::startAnalysis(AnalysisResult* i) {
    analysisResult = i;
    ready = false;
    line = 0;
}
void
XpmLineAnalyzer::handleLine(const char* data, uint32_t length) {
    if (ready) return;
    ++line;
    if (line == 1 && (length < 9 || strncmp(data, "/* XPM */", 9))) {
        // this is not an xpm file
        ready = true;
        return;
    } else if (length == 0 || data[0] != '"') {
        return;
    }
    uint32_t i = 0;
    // we have found the line which should contain the information we want
    ready = true;
    // read the height
    uint32_t propertyValue = 0;
    i++;
    while (i < length && isdigit(data[i])) {
        propertyValue = (propertyValue * 10) + data[i] - '0';
        i++;
    }

    if (i >= length || data[i] != ' ')
        return;

    analysisResult->addValue(factory->heightField, propertyValue);

    // read the width
    propertyValue = 0;
    i++;
    while (i < length && isdigit(data[i])) {
        propertyValue = (propertyValue * 10) + data[i] - '0';
        i++;
    }

    if (i >= length || data[i] != ' ')
        return;

    analysisResult->addValue(factory->widthField, propertyValue);

    // read the number of colors
    propertyValue = 0;
    i++;
    while (i < length && isdigit(data[i])) {
        propertyValue = (propertyValue * 10) + data[i] - '0';
        i++;
    }

    if (i >= length || data[i] != ' ')
        return;

    analysisResult->addValue(factory->numberOfColorsField, propertyValue);
    analysisResult->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image");
}
bool
XpmLineAnalyzer::isReadyWithStream() {
    return ready;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamLineAnalyzerFactory*>
    streamLineAnalyzerFactories() const {
        list<StreamLineAnalyzerFactory*> af;
        af.push_back(new XpmLineAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
