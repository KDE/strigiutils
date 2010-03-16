/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "bmpendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <sstream>
#include <cstring>
using namespace std;
using namespace Strigi;

void
BmpEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#formatSubtype");
    compressionField = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm");
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    colorDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    rdftypeField = reg.typeField;

    addField(typeField);
    addField(compressionField);
    addField(widthField);
    addField(heightField);
    addField(colorDepthField);
    addField(rdftypeField);
}

bool
BmpEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    bool ok = false;
    if (headersize > 2) {
        ok = strncmp(header, "BM", 2) == 0
            || strncmp(header, "BA", 2) == 0
            || strncmp(header, "CI", 2) == 0
            || strncmp(header, "CP", 2) == 0
            || strncmp(header, "IC", 2) == 0
            || strncmp(header, "PT", 2) == 0;
    }
    return ok;
}
signed char
BmpEndAnalyzer::analyze(AnalysisResult& rs, InputStream* in) {
    // read BMP file type and ensure it is not damaged
    /*
    const char * bmptype_bm = "BM";
    const char * bmptype_ba = "BA";
    const char * bmptype_ci = "CI";
    const char * bmptype_cp = "CP";
    const char * bmptype_ic = "IC";
    const char * bmptype_pt = "PT";
    */

    const char* bmp_id;
    in->read(bmp_id, 2, 2);
    in->reset(0);
/* //FIXME: either get rid of this or replace with NIE equivalent
    if (memcmp(bmp_id, bmptype_bm, 2) == 0) {
        rs.addValue(factory->typeField, "Windows Bitmap");
    } else if (memcmp(bmp_id, bmptype_ba, 2) == 0) {
        rs.addValue(factory->typeField, "OS/2 Bitmap Array");
    } else if (memcmp(bmp_id, bmptype_ci, 2) == 0) {
        rs.addValue(factory->typeField, "OS/2 Color Icon");
    } else if (memcmp(bmp_id, bmptype_cp, 2) == 0) {
        rs.addValue(factory->typeField, "OS/2 Color Pointer");
    } else if (memcmp(bmp_id, bmptype_ic, 2) == 0) {
        rs.addValue(factory->typeField, "OS/2 Icon");
    } else if (memcmp(bmp_id, bmptype_pt, 2) == 0) {
        rs.addValue(factory->typeField, "OS/2 Pointer");
    } else {
        return -1;
    }
*/
    // read compression type (bytes #30-33)
    const char* h;
    int32_t n = in->read(h, 34, 34);
    in->reset(0);
    if (n < 34) return -1;

    uint32_t width = readLittleEndianUInt32(h+18);
    rs.addValue(factory->widthField, width);
    uint32_t height = readLittleEndianUInt32(h+22);
    rs.addValue(factory->heightField, height);
    uint32_t colorDepth = readLittleEndianUInt16(h+28);
    rs.addValue(factory->colorDepthField, colorDepth);
/* //FIXME: either get rid of this or replace with NIE equivalent
    uint32_t bmpi_compression = readLittleEndianUInt32(h+30);
    switch (bmpi_compression) {
    case 0 :
        rs.addValue(factory->compressionField, "None");
        break;
    case 1 :
        rs.addValue(factory->compressionField, "RLE 8bit/pixel");
        break;
    case 2 :
        rs.addValue(factory->compressionField, "RLE 4bit/pixel");
        break;
    case 3 :
        rs.addValue(factory->compressionField, "Bitfields");
        break;
    default :
        rs.addValue(factory->compressionField, "Unknown");
    }
*/
    rs.addValue(factory->rdftypeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");

    return 0;
}

