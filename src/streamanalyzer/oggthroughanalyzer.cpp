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

#include "oggthroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include "analysisresult.h"
#include "textutils.h"
#include <iostream>
#include <cctype>
#include <cstring>
using namespace Strigi;
using namespace std;

void
OggThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    fields["title"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#title");
    fields["album"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#album");
    fields["artist"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#artist");
    fields["genre"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#genre");
    fields["codec"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#audioCodec");
    fields["composer"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#composer");
    fields["performer"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#performer");
    fields["date"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#contentCreated");
    fields["description"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#description");
    fields["tracknumber"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#trackNumber");


    fields["version"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#version");
    fields["isrc"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#isrc");
    fields["copyright"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#copyright");
    fields["license"] = r.registerField("http://freedesktop.org/standards/xesam/1.0/core#license");

// fields left unimplemented: ORGANIZATION, LOCATION, CONTACT

}

void
OggThroughAnalyzer::setIndexable(AnalysisResult* i) {
    indexable = i;
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
    // check the header for signatures
    // the first ogg page starts at position 0, the second at position 58
    if (nread < nreq || strcmp("OggS", buf) || strcmp("vorbis", buf+29)
            || strcmp("OggS", buf+58)) {
        return in;
    }
    // read the number of page segments at 58 + 26
    unsigned char segments = (unsigned char)buf[84];
    if (85 + segments >= nread) {
        // this cannot be a good vorbis file: the initial page is too large
        return in;
    }

    // read the sum of page segment sizes 
    int psize = 0;
    for (int i=0; i<segments; ++i) {
        psize += (unsigned char)buf[85+i];
    }
    // read the entire first two pages
    nreq = 85 + segments + psize;
    nread = in->read(buf, nreq, nreq);
    in->reset(0);
    if (nread < nreq) {
        return in;
    }
    // we have now read the second Ogg Vorbis header containing the comments
    // get a pointer to the first page segment in the second page
    const char* p2 = buf + 85 + segments;
    // check the header of the 'vorbis' page
    if (psize < 15 || strncmp(p2 + 1, "vorbis", 6)) {
        return in;
    }
    // get a pointer to the end of the second page
    const char* end = p2 + psize;
    uint32_t size = readLittleEndianUInt32(p2+7);
    // advance to the position of the number of fields and read it
    p2 += size + 11;
    if (p2 + 4 > end) {
        return in;
    }
    uint32_t nfields = readLittleEndianUInt32(p2);
    // read all the comments
    p2 += 4;
    for (uint32_t i = 0; p2 < end && i < nfields; ++i) {
        // read the comment length
        size = readLittleEndianUInt32(p2);
        p2 += 4;
        if (size <= (uint32_t)(end - p2)) {
            uint32_t eq = 1;
            while (eq < size && p2[eq] != '=') eq++;
            if (size > eq) {
                string name(p2, eq);
                // convert field name to lower case
                const int length = name.length();
                for(int k=0; k!=length; ++k) {
                    name[k] = std::tolower(name[k]);
                }
                // check if we can handle this field and if so handle it
                map<string, const RegisteredField*>::const_iterator iter
                    = factory->fields.find(name);
                if (iter != factory->fields.end()) {
                    string value(p2+eq+1, size-eq-1);
                    indexable->addValue(iter->second, value);
                }
            }
        } else {
            cerr << "problem with tag size of " << size << endl;
            return in;
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
