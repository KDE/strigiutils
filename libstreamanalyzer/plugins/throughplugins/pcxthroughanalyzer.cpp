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
#include <strigi/fieldtypes.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>

using namespace Strigi;
using namespace std;

void
PcxThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    hResolutionField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#horizontalResolution");
    vResolutionField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#verticalResolution");
    colorDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    typeField = reg.typeField;

    addField(compressionField);
    addField(widthField);
    addField(heightField);
    addField(hResolutionField);
    addField(vResolutionField);
    addField(colorDepthField);
    addField(typeField);
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
    /* //FIXME: either get rid of this or replace with NIE equivalent
    if ( header[2] == 1 ) {
	indexable->addValue(factory->compressionField, "RLE");
    } else {
	indexable->addValue(factory->compressionField, "None");
    }
    */
    indexable->addValue(factory->hResolutionField, (int32_t)readLittleEndianUInt16(header+12));
    indexable->addValue(factory->vResolutionField, (int32_t)readLittleEndianUInt16(header+14));

    indexable->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");
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
