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
#include "jstreamsconfig.h"
#include "bmpendanalyzer.h"
#include "indexable.h"
#include <sstream>
using namespace std;
using namespace jstreams;

bool
BmpEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    bool ok = false;
    if (headersize > 2) {
        ok |= strncmp(header, "BM", 2) == 0;
        ok |= strncmp(header, "BA", 2) == 0;
        ok |= strncmp(header, "CI", 2) == 0;
        ok |= strncmp(header, "CP", 2) == 0;
        ok |= strncmp(header, "IC", 2) == 0;
        ok |= strncmp(header, "PT", 2) == 0;
    }
    return ok;
}
char
BmpEndAnalyzer::analyze(jstreams::Indexable& idx, jstreams::InputStream* in) {
    // read BMP file type and ensure it is not damaged
    const char * bmptype_bm = "BM";
    const char * bmptype_ba = "BA";
    const char * bmptype_ci = "CI";
    const char * bmptype_cp = "CP";
    const char * bmptype_ic = "IC";
    const char * bmptype_pt = "PT";

    const char* bmp_id;
    in->read(bmp_id, 2, 2);
    in->reset(0);

    if (memcmp(bmp_id, bmptype_bm, 2) == 0) {
        idx.setField("Type", "Windows Bitmap");
    } else if (memcmp(bmp_id, bmptype_ba, 2) == 0) {
        idx.setField("Type", "OS/2 Bitmap Array");
    } else if (memcmp(bmp_id, bmptype_ci, 2) == 0) {
        idx.setField("Type", "OS/2 Color Icon");
    } else if (memcmp(bmp_id, bmptype_cp, 2) == 0) {
        idx.setField("Type", "OS/2 Color Pointer");
    } else if (memcmp(bmp_id, bmptype_ic, 2) == 0) {
        idx.setField("Type", "OS/2 Icon");
    } else if (memcmp(bmp_id, bmptype_pt, 2) == 0) {
        idx.setField("Type", "OS/2 Pointer");
    } else {
        return -1;
    }

    // read compression type (bytes #30-33)
    const char* h;
    int32_t n = in->read(h, 34, 34);
    in->reset(0);
    if (n < 34) return -1;

    uint32_t bmpi_compression = (unsigned char)h[33] + ((unsigned char)h[32]<<8)
         + ((unsigned char)h[31]<<16) + ((unsigned char)h[30]<<24);

    switch (bmpi_compression) {
    case 0 :
        idx.setField("Compression", "None");
        break;
    case 1 :
        idx.setField("Compression", "RLE 8bit/pixel");
        break;
    case 2 :
        idx.setField("Compression", "RLE 4bit/pixel");
        break;
    case 3 :
        idx.setField("Compression", "Bitfields");
        break;
    default :
        idx.setField("Compression", "Unknown");
    }

    return 0;
}

