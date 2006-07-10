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
#include "zipendanalyzer.h"
#include "zipinputstream.h"
#include "streamindexer.h"
#include "subinputstream.h"
using namespace jstreams;

bool
ZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return ZipInputStream::checkHeader(header, headersize);
}
char
ZipEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable*) {
    ZipInputStream zip(in);
    InputStream *s = zip.nextEntry();
    if (zip.getStatus()) {
        printf("error: %s\n", zip.getError());
//        exit(1);
    }
    while (s) {
        std::string file = filename+'/';
        file += zip.getEntryInfo().filename;
        indexer->analyze(file, zip.getEntryInfo().mtime, s, depth);
        s = zip.nextEntry();
    }
    if (zip.getStatus() == jstreams::Error) {
        error = zip.getError();
        return -1;
    } else {
        error.resize(0);
    }
    return 0;
}

