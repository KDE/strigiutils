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

#include "strigiconfig.h"
#include "id3v2throughanalyzer.h"
#include "analysisresult.h"
using namespace Strigi;
using namespace std;

const string ID3V2ThroughAnalyzerFactory::titleFieldName("audio.title");
const string ID3V2ThroughAnalyzerFactory::artistFieldName("audio.artist");
const string ID3V2ThroughAnalyzerFactory::albumFieldName("audio.album");

void
ID3V2ThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField = r.registerField(titleFieldName, FieldRegister::stringType,
        1, 0);
    artistField = r.registerField(artistFieldName, FieldRegister::stringType,
        1, 0);
    albumField = r.registerField(albumFieldName, FieldRegister::stringType,
        1, 0);
}

void
ID3V2ThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}
int32_t
readSize(const unsigned char* b, bool async) {
    const char* c = (const char*)b;
    if (async) {
        if (c[0] < 0 || c[1] < 0 || c[2] < 0 || c[3] < 0) {
            return -1;
        }
        return (((int32_t)b[0])<<21) + (((int32_t)b[1])<<14)
                + (((int32_t)b[2])<<7) + ((int32_t)b[3]);
    }
    return (((int32_t)b[0])<<24) + (((int32_t)b[1])<<16)
            + (((int32_t)b[2])<<8) + ((int32_t)b[3]);
}
InputStream*
ID3V2ThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in)
        return in;

    // read 10 byte header
    const char* buf;
    int32_t nread = in->read(buf, 10, 10);
    in->reset(0);
    if (nread == 10 && strncmp("ID3", buf, 3) == 0 // check that it's ID3
            && buf[3] >= 0 && buf[3] <= 4  // only handle version <= 4
            && buf[5] == 0 // we're too dumb too handle flags
            ) {
        bool async = buf[3] >= 4;

        // calculate size from 4 syncsafe bytes
        int32_t size = readSize((unsigned char*)buf+6, async);
        if (size < 0 || size > 100000) return in;
        size += 10; // add the size of the header

        // read the entire tag
        nread = in->read(buf, size, size);
        in->reset(0);
        if (nread != size) {
            return in;
        }
        const char* p = buf + 10;
        buf += size;
        while (indexable && p < buf && *p) {
            size = readSize((unsigned char*)p+4, async);
            if (size < 0 || size > (buf-p)-11) {
                // fprintf(stderr, "size < 0: %i\n", size);
                return in;
            }
            if (p[10] == 0 || p[10] == 1) { // text is ISO-8859-1 or utf8
                if (strncmp("TIT2", p, 4) == 0) {
                    indexable->addValue(factory->titleField,
                        string(p+11, size-1));
                } else if (strncmp("TPE1", p, 4) == 0) {
                    indexable->addValue(factory->artistField,
                        string(p+11, size-1));
                } else if (strncmp("TALB", p, 4) == 0) {
                    indexable->addValue(factory->albumField,
                        string(p+11, size-1));
                }
            }
            p += size + 10;
        }
    }
    return in;
}
bool
ID3V2ThroughAnalyzer::isReadyWithStream() {
    return true;
}
