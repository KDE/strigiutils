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

#include "authroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include "../../lib/rdfnamespaces.h"

using namespace std;
using namespace Strigi;

const string
    musicClassName(
	NMM_DRAFT "MusicPiece");

void
AuThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    lengthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#duration");
    sampleRateField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#sampleRate");
    channelsField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#channels");
    sampleBitDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/nfo#bitsPerSample");
    sampleDataTypeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#audioSampleDataType");
    typeField = reg.typeField;

    addField(lengthField);
    addField(sampleRateField);
    addField(channelsField);
    addField(sampleBitDepthField);
    addField(sampleDataTypeField);
    addField(typeField);
}

// Analyzer
void
AuThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
AuThroughAnalyzer::connectInputStream(InputStream* in) {
    if (in == 0) return in;
    const int32_t nreq = 24;
    const char* buf;
    int32_t nread = in->read(buf, nreq, nreq);
    in->reset(0);

    if (nread < nreq) {
        return in;
    }

    /*
     * description of the 'au'/u-law format:
     * http://sox.sourceforge.net/AudioFormats-11.html#ss11.2
     */

    // the file signature
    const char sign[] = { 0x2e, 0x73, 0x6e, 0x64 };
    if (strncmp(sign, buf, 4)) {
        return in;
    }

    // we skip 8 bytes: 4 for the magic number + 4 for data pointer
    uint32_t dataSize = readBigEndianUInt32(buf+8);
    //uint32_t encoding = readBigEndianUInt32(buf+12);
    uint32_t sampleRate = readBigEndianUInt32(buf+16);
    uint32_t channels = readBigEndianUInt32(buf+20);

    analysisResult->addValue(factory->sampleRateField, sampleRate);
    analysisResult->addValue(factory->channelsField, channels);

    uint16_t bytesPerSample = 0;
    /* //FIXME: either get rid of this or replace with NIE equivalent
    switch (encoding) {
    case 1 :
        analysisResult->addValue(factory->sampleDataTypeField, "ISDN u-law");
        bytesPerSample = 1;
        break;
    case 2 :
        analysisResult->addValue(factory->sampleDataTypeField, "linear PCM [REF-PCM]");
        bytesPerSample = 1;
        break;
    case 3 :
        analysisResult->addValue(factory->sampleDataTypeField, "linear PCM");
        bytesPerSample = 2;
        break;
    case 4 :
        analysisResult->addValue(factory->sampleDataTypeField, "linear PCM");
        bytesPerSample = 3;
        break;
    case 5 :
        analysisResult->addValue(factory->sampleDataTypeField, "linear PCM");
        bytesPerSample = 4;
        break;
    case 6 :
        analysisResult->addValue(factory->sampleDataTypeField, "IEEE floating point");
        bytesPerSample = 4;
        break;
    case 7 :
        analysisResult->addValue(factory->sampleDataTypeField, "IEEE floating point");
        bytesPerSample = 8;
        break;
    case 23 :
        analysisResult->addValue(factory->sampleDataTypeField, "ISDN u-law compressed");
        bytesPerSample = 1;
        break;
    default :
        analysisResult->addValue(factory->sampleDataTypeField, "Unknown");
        bytesPerSample = 0;
    }
    */
    if(bytesPerSample) {
        analysisResult->addValue(factory->sampleBitDepthField, bytesPerSample*8);
    }
    // work out length from bytespersample + channels + size
    if ((0 < channels) && (0 < dataSize) && (dataSize != 0xFFFFFFFF) && (0 < bytesPerSample) && (0 < sampleRate)) {
        uint32_t length = dataSize / channels / bytesPerSample / sampleRate;
        analysisResult->addValue(factory->lengthField, length);
    }

    analysisResult->addValue(factory->typeField, musicClassName);

    return in;
}

bool
AuThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new AuThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
