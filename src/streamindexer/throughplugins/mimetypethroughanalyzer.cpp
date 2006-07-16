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
#include "strigi_plugins.h"

#include "mimetypethroughanalyzer.h"
#include "indexwriter.h"
using namespace std;

MimeTypeThroughAnalyzer::MimeTypeThroughAnalyzer() {
    magic = magic_open(MAGIC_MIME);
    if ( magic_load(magic, "magic") == -1 ){
        printf("magic_load: %s\n",magic_error(magic));
    }
}
MimeTypeThroughAnalyzer::~MimeTypeThroughAnalyzer() {
    magic_close(magic);
}
jstreams::InputStream *
MimeTypeThroughAnalyzer::connectInputStream(jstreams::InputStream *in) {
    // determine the mimetype
    const char* mime;
    int64_t pos = in->getPosition();
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


//define all the available analyzers in this plugin
STRIGI_THROUGH_PLUGINS_START();
STRIGI_THROUGH_PLUGINS_REGISTER(MimeTypeThroughAnalyzer);
STRIGI_THROUGH_PLUGINS_END();
