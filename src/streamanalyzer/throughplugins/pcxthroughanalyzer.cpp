/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Jakub Stachowski <qbast@go2.pl>
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

#include "pcxthroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include "fieldtypes.h"
#include "analysisresult.h"
#include "textutils.h"

using namespace Strigi;
using namespace std;

const string PcxThroughAnalyzerFactory::compressionFieldName("http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
const string PcxThroughAnalyzerFactory::widthFieldName("http://freedesktop.org/standards/xesam/1.0/core#width");
const string PcxThroughAnalyzerFactory::heightFieldName("http://freedesktop.org/standards/xesam/1.0/core#height");
const string PcxThroughAnalyzerFactory::colorDepthFieldName("http://freedesktop.org/standards/xesam/1.0/core#pixelDataBitDepth");
const string PcxThroughAnalyzerFactory::hResolutionFieldName("http://freedesktop.org/standards/xesam/1.0/core#horizontalResolution");
const string PcxThroughAnalyzerFactory::vResolutionFieldName("http://freedesktop.org/standards/xesam/1.0/core#verticalResolution");


void
PcxThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    compressionField = reg.registerField(compressionFieldName);
    widthField = reg.registerField(widthFieldName);
    heightField = reg.registerField(heightFieldName);
    hResolutionField = reg.registerField(hResolutionFieldName);
    vResolutionField = reg.registerField(vResolutionFieldName);
    colorDepthField = reg.registerField(colorDepthFieldName);

    typeField = reg.typeField;
}

void
PcxThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}

InputStream*
PcxThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in)
        return in;

    const char* header;
    // read up to NPlanes field
    int32_t nreq = 128;
    int32_t nread = in->read(header, nreq, nreq);
    in->reset(0);
    if (nread < nreq) return in;
    // check header for some magic bytes and determine if it is valid pcx file
    if (header[0]!=10 || header[1]>5 || header[1]==1 || header[2]>1 || 
	header[3]>8 || header[3]==3 || (header[3]>4 && header[3]<8) || header[64]!=0) return in;
    // header should be padded to 128 bytes with zeros
    for (int i=74;i<128;i++) if (header[i]!=0) return in;
    
    int w = ( readLittleEndianUInt16(header+8)-readLittleEndianUInt16(header+4) ) + 1;
    int h = ( readLittleEndianUInt16(header+10)-readLittleEndianUInt16(header+6) ) + 1;
    int bpp = header[3]*header[65];

    indexable->addValue(factory->widthField, (int32_t)w);
    indexable->addValue(factory->heightField, (int32_t)h);
    indexable->addValue(factory->colorDepthField, (int32_t)bpp);
    if ( header[2] == 1 ) {
	indexable->addValue(factory->compressionField, "RLE");
    } else {
	indexable->addValue(factory->compressionField, "None");
    }
    indexable->addValue(factory->hResolutionField, (int32_t)readLittleEndianUInt16(header+12));
    indexable->addValue(factory->vResolutionField, (int32_t)readLittleEndianUInt16(header+14));

    indexable->addValue(factory->typeField, "http://freedesktop.org/standards/xesam/1.0/core#Image");
    return in;
}
bool
PcxThroughAnalyzer::isReadyWithStream() {
    return true;
}

//Factory
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new PcxThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
