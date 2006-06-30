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
#include "textendanalyzer.h"
#include "streamindexer.h"
#include "inputstreamreader.h"
#include "indexwriter.h"
using namespace jstreams;

bool
TextEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    const char* end = header + headersize;
    const char* p = header-1;
    while (++p < end) {
        if (*p <= 8) {
            return false;
        }
    }
    return true;
}

char
TextEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, Indexable* i) {
    // pass a piece of text to the indexer. it's up to the indexer to break
    // it down into words
    const char* b;
    int32_t nread = in->read(b, 1024*1024, 0);
    if (nread > 0) {
        i->addText(b, nread);
    }
    if (nread != Eof) {
        error = in->getError();
        // TODO investigate
        //return -1;
    }
//    InputStreamReader reader(in);
//    i->addStream("content", &reader);
    return 0;
}
