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

#include "strigiconfig.h"
#include "fieldtypes.h"
#include "pcxthroughanalyzer.h"
#include "analysisresult.h"
#include "textutils.h"

using namespace Strigi;
using namespace std;

const string PcxThroughAnalyzerFactory::compressionFieldName("compression");
const string PcxThroughAnalyzerFactory::widthFieldName("image.dimensions.x");
const string PcxThroughAnalyzerFactory::heightFieldName("image.dimensions.y");
const string PcxThroughAnalyzerFactory::colorDepthFieldName("image.color_depth");
const string PcxThroughAnalyzerFactory::hResolutionFieldName("image.resolution.x");
const string PcxThroughAnalyzerFactory::vResolutionFieldName("image.resolution.y");


void
PcxThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    compressionField = reg.registerField(compressionFieldName,
        FieldRegister::stringType, 1, 0);
    widthField = reg.registerField(widthFieldName,
        FieldRegister::integerType, 1, 0);
    heightField = reg.registerField(heightFieldName,
        FieldRegister::integerType, 1, 0);
    hResolutionField = reg.registerField(hResolutionFieldName,
        FieldRegister::integerType, 1, 0);
    vResolutionField = reg.registerField(vResolutionFieldName,
        FieldRegister::integerType, 1, 0);
    colorDepthField = reg.registerField(colorDepthFieldName,
        FieldRegister::integerType, 1, 0);
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

    indexable->addValue(factory->widthField, w);
    indexable->addValue(factory->heightField, h);
    indexable->addValue(factory->colorDepthField, bpp);
    if ( header[2] == 1 ) {
	indexable->addValue(factory->compressionField, "RLE");
    } else {
	indexable->addValue(factory->compressionField, "None");
    }
    indexable->addValue(factory->hResolutionField, readLittleEndianUInt16(header+12));
    indexable->addValue(factory->vResolutionField, readLittleEndianUInt16(header+14));

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
