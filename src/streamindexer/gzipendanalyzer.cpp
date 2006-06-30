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
#include "gzipendanalyzer.h"
#include "gzipinputstream.h"
#include "tarendanalyzer.h"
#include "tarinputstream.h"
#include "tarendanalyzer.h"
#include "streamindexer.h"
#include "indexwriter.h"
using namespace jstreams;
using namespace std;

bool
GZipEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return headersize > 2 && (unsigned char)header[0] == 0x1f
        && (unsigned char)header[1] == 0x8b;
}
char
GZipEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    GZipInputStream stream(in);
    // since this is gzip file, its likely that it contains a tar file
    const char* start;
    int32_t nread = stream.read(start, 1024, 0);
    if (nread < -1) {
        printf("Error reading gzip: %s\n", stream.getError());
        return -2;
    }
    stream.reset(0);
    if (TarInputStream::checkHeader(start, nread)) {
        return TarEndAnalyzer::staticAnalyze(filename, &stream, depth, indexer,
            idx);
    } else {
        std::string file;
        uint p1 = filename.rfind("/");
        if (p1 != string::npos) {
            int len = filename.length();
            if (len > 3 && filename.substr(len-3) == ".gz") {
                file = filename + filename.substr(p1,len-p1-3);
            } else {
                file = filename + filename.substr(p1);
            }
        } else {
            // last resort
            file = filename+"/gunzipped";
        }
        return indexer->analyze(file, idx->getMTime(), &stream, depth);
    }
}
