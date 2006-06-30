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
#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "streamindexer.h"
#include "subinputstream.h"
using namespace jstreams;

bool
TarEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return TarInputStream::checkHeader(header, headersize);
}
char
TarEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    return staticAnalyze(filename, in, depth, indexer, idx);
}
char
TarEndAnalyzer::staticAnalyze(std::string filename, jstreams::InputStream *in,
        int depth, jstreams::StreamIndexer *indexer, jstreams::Indexable*) {
    TarInputStream tar(in);
    InputStream *s = tar.nextEntry();
    while (s) {
        std::string file = filename+"/";
        file += tar.getEntryInfo().filename;
        indexer->analyze(file, tar.getEntryInfo().mtime, s, depth);
        s = tar.nextEntry();
    }
    if (tar.getStatus() == jstreams::Error) {
        return -1;
//        printf("Error: %s\n", tar.getError());
    } else {
//        printf("finished ok\n");
    }
    return 0;
}

