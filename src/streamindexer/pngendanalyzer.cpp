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
#include "pngendanalyzer.h"
#include "streamindexer.h"
#include "indexwriter.h"
#include <sstream>
using namespace std;
using namespace jstreams;

bool
PngEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const char pngmagic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    return headersize >= 24 &&  memcmp(header, pngmagic, 8) == 0;
}
char
PngEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* i) {
    const char* h;
    int32_t n = in->read(h, 24, 24);
    if (n < 24) return -1;
    in->reset(0);
  
    // perform magic to access png dimensions
    int32_t x = (unsigned char)h[19] + ((unsigned char)h[18]<<8)
         + ((unsigned char)h[17]<<16) + ((unsigned char)h[16]<<24);
    int32_t y = (unsigned char)h[23] + ((unsigned char)h[22]<<8)
         + ((unsigned char)h[21]<<16) + ((unsigned char)h[20]<<24);
    ostringstream v;
    v << x;
    i->setField("width", v.str());
    v.str("");
    v << y;
    i->setField("height", v.str());
    return 0;
}

