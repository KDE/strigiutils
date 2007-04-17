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

#include "strigiconfig.h"
#include "xbmthroughanalyzer.h"
#include "analysisresult.h"
#include "fieldtypes.h"

using namespace std;
using namespace Strigi;

// AnalyzerFactory
const string XbmThroughAnalyzerFactory::widthFieldName("image.width");
const string XbmThroughAnalyzerFactory::heightFieldName("image.height");
const string XbmThroughAnalyzerFactory::xHotFieldName("xhot");
const string XbmThroughAnalyzerFactory::yHotFieldName("yhot");

void
XbmThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    widthField = reg.registerField(widthFieldName,
        FieldRegister::integerType, 1, 0);
    heightField = reg.registerField(heightFieldName,
        FieldRegister::integerType, 1, 0);
    xHotField = reg.registerField(xHotFieldName,
        FieldRegister::integerType, 1, 0);
    yHotField = reg.registerField(yHotFieldName,
        FieldRegister::integerType, 1, 0);
}

// Analyzer
void
XbmThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

/**
 * @returns the next offset, @c -1 if an error occurred.
 */
int32_t
processLine(const char* buf, int32_t bufSize, int32_t offset, int32_t& data) {
    // check we have a '#define'
    if (bufSize < offset+7 || strncmp(buf + offset, "#define", 7)) {
        return -1;
    }

    // skip spaces
    int32_t i = offset + 7;
    while (i < bufSize && (buf[i] == ' ' || buf[i] == '\t')) {
        i++;
    }
    if (i >= bufSize)
        return -1;

    // skip the name of the define (should be /.*_width/ or /.*_height/ or /.*_x_hot/ or /.*_y_hot/)
    while (i < bufSize && !(buf[i] == ' ' || buf[i] == '\t')) {
        i++;
    }
    if (i >= bufSize)
        return -1;

    // skip spaces
    while (i < bufSize && (buf[i] == ' ' || buf[i] == '\t')) {
        i++;
    }
    // check if we have a valid data
    if (i >= bufSize || !isdigit(buf[i]))
        return -1;

    // read the data
    data = 0;
    while (i < bufSize && isdigit(buf[i])) {
        data = (data * 10) + buf[i] - '0';
        i++;
    }
    if (i >= bufSize)
        return -1;

    // skip eol
    while (i < bufSize && (buf[i] == '\n' || buf[i] == '\r')) {
        i++;
    }

    return i;
}

InputStream*
XbmThroughAnalyzer::connectInputStream(InputStream* in) {
    const int32_t nreq = 16;
    int32_t nextOffset = 0;
    const char* buf;

    int32_t nread = in->read(buf, nreq, -1);
    in->reset(0);

    if (nread < nreq) {
        return in;
    }

    int32_t width;
    nextOffset = processLine(buf, nread, nextOffset, width);
    if (nextOffset == -1) {
        return in;
    }
    int32_t height;
    nextOffset = processLine(buf, nread, nextOffset, height);
    if (nextOffset == -1) {
        return in;
    }

    analysisResult->addValue(factory->widthField, width);
    analysisResult->addValue(factory->heightField, height);

    // these both fields are optional
    int32_t xHot;
    nextOffset = processLine(buf, nread, nextOffset, xHot);
    if (nextOffset != -1) {
        int32_t yHot;
        nextOffset = processLine(buf, nread, nextOffset, yHot);
        if (nextOffset != -1) {
            analysisResult->addValue(factory->xHotField, xHot);
            analysisResult->addValue(factory->yHotField, yHot);
        }
    }

    return in;
}

bool
XbmThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new XbmThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
