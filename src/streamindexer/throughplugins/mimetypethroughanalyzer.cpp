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
#define STRIGI_IMPORT_API //todo: could also define this in cmake...
#include "jstreamsconfig.h"

#include "mimetypethroughanalyzer.h"
#include "indexable.h"
using namespace std;
using namespace jstreams;

MimeTypeThroughAnalyzer::MimeTypeThroughAnalyzer() {
    magic = magic_open(MAGIC_MIME);
    if ( magic_load(magic, 0) == -1 )
		if ( magic_load(magic, "magic") == -1 )
				fprintf(stderr, "magic_load: %s\n",magic_error(magic));

}
MimeTypeThroughAnalyzer::~MimeTypeThroughAnalyzer() {
    magic_close(magic);
}
::InputStream *
MimeTypeThroughAnalyzer::connectInputStream(::InputStream *in) {
    // determine the mimetype
    const char* mime;
    int64_t pos = in->getPosition();
    // min == 1 and max == 0 means: 'use whatever buffer size you have already
    int32_t n = in->read(mime, 1, 0);
    in->reset(pos);
    if (n >= 0) {
        mime = magic_buffer(magic, mime, n);
        if (mime == 0) return in;
        const char* slash = strchr(mime, '/');
        if (slash == 0) return in;
        int l = strcspn(mime, " \t;\\");
        if (l < slash-mime) return in;
        idx->setMimeType(string(mime, l));
        const char* charset = strstr(mime+l, "charset=");
        if (charset == 0) return in;
        charset += 8;
        idx->setEncoding(charset);
    }
    return in;
}
bool
MimeTypeThroughAnalyzer::isReadyWithStream() {
    return true;
}

class MimeTypeThroughAnalyzerFactory
    : public StreamThroughAnalyzerFactory {
private:
    const char* getName() const {
        return "MimeTypeThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new MimeTypeThroughAnalyzer();
    }
};

class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    getStreamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new MimeTypeThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)

