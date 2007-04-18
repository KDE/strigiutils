/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias Pfeiffer <tgpfeiffer@web.de>
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
#include "oggthroughanalyzer.h"
#include "analysisresult.h"
#include <cctype>
using namespace Strigi;
using namespace std;

void
OggThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    fields["title"] = r.registerField("audio.title", FieldRegister::stringType, 1, 0);
    fields["album"] = r.registerField("audio.album", FieldRegister::stringType, 1, 0);
    fields["artist"] = r.registerField("audio.artist", FieldRegister::stringType,
        1, 0);
    fields["genre"] = r.registerField("content.genre", FieldRegister::stringType, 1, 0);
    fields["codec"] = r.registerField("media.codec", FieldRegister::stringType, 1, 0);
}

void
OggThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
}
int32_t
readSize(const char*b) {
    return (int32_t)b[0] + (int32_t)(b[1]<<8) + (int32_t)(b[2]<<16)
        + (int32_t)(b[3]<<24);
}
InputStream*
OggThroughAnalyzer::connectInputStream(InputStream* in) {
    if(!in) {
        return in;
    }

    const char* buf;
    // read 1024 initially
    int32_t nreq = 1024;
    int32_t nread = in->read(buf, nreq, nreq);
    in->reset(0);
    if (nread < nreq || strcmp("OggS", buf) || strcmp("vorbis", buf+29)
            || strcmp("OggS", buf+58)) {
        return in;
    }
    unsigned char packets = (unsigned char)buf[84];
    if (85 + packets >= nread) {
        // this cannot be a good vorbis file
        return in;
    }
 
    int psize = 0;
    for (int i=0; i<packets; ++i) {
        psize += (unsigned char)buf[85+i];
    }
    nreq = psize + 85 + packets;
    nread = in->read(buf, nreq, nreq);
    in->reset(0);
    if (nread < nreq) {
        return in;
    }
    // we have now read the second Ogg Vorbis header containing the comments
    const char* p2 = buf + 85 + packets;
    const char* end = p2 + psize;
    if (psize < 15 || strncmp(p2 + 1, "vorbis", 6)) {
        return in;
    }
    int32_t size = readSize(p2+7);
    p2 += size + 11;
    if (p2 + 4 > end) {
        return in;
    }
    int32_t nfields = readSize(p2);
    p2 += 4;
    for (int32_t i = 0; p2 < end && i < nfields; ++i) {
        size = readSize(p2);
        p2 += 4;
        if (p2 + size < end) {
            int32_t eq = 1;
            while (eq < size && p2[eq] != '=') eq++;
            if (eq < size - 1) {
                string name(p2, eq);
                string value(p2+eq+1, size-eq-1);
                // convert field name to lower case
                const int length = name.length();
                for(int k=0; k!=length; ++k) {
                    name[k] = std::tolower(name[k]);
                }
                map<string, const RegisteredField*>::const_iterator iter
                    = factory->fields.find(name);
                if (iter != factory->fields.end()) {
                    indexable->addValue(iter->second, value);
                }
            }
        }
        p2 += size;
    }
    // set the "codec" value
    indexable->addValue(factory->fields.find("codec")->second, "Ogg/Vorbis");
    return in;
}
bool
OggThroughAnalyzer::isReadyWithStream() {
    return true;
}
