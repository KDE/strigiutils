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
#include "arinputstream.h"
#include "subinputstream.h"


using namespace jstreams;

bool
ArInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x21,0x3c,0x61,0x72,0x63,0x68,0x3e,0x0a};
    if (datasize < 8) return false;
    bool ok = memcmp(data, magic, 8) == 0;
    return ok;
}
ArInputStream::ArInputStream(StreamBase<char>* input)
        : SubStreamProvider(input) {
    entrystream = 0;
    // check header
    const char*b;
    if (input->read(b, 8, 8) != 8 || !checkHeader(b, 8)) {
        status = Error;
    }
}
ArInputStream::~ArInputStream() {
    if (entrystream) {
        delete entrystream;
    }
}
StreamBase<char>*
ArInputStream::nextEntry() {
    if (status) return 0;
    if (entrystream) {
        entrystream->skip(entrystream->getSize());
        delete entrystream;
        entrystream = 0;
    }
    readHeader();
    if (status) return 0;
    entrystream = new SubInputStream(input, entryinfo.size);
    return entrystream;
}
void
ArInputStream::readHeader() {
    const char *b;
    int32_t toread;
    int32_t nread;

    int64_t pos = input->getPosition();
    if (pos%2) {
        input->skip(1);
    }

    // read the first 60 characters
    toread = 60;
    nread = input->read(b, toread, toread);
    if (nread <= 1) { // allow for a closing byte
        status = Eof;
        return;
    }
    if (nread != toread) {
        error = "Error reading ar header: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        status = Error;
        return;
    }
    int len;
    for (len=0; len<16; len++) {
        char c = b[len];
        if (c == ' ' || c == '/' || c == '\0') {
            break;
        }
    }

    entryinfo.size = atoi(b+48);
    entryinfo.mtime = atoi(b+16);
    if (len == 0) {
        if (b[1] == '/') {
            nread = input->read(b, entryinfo.size, entryinfo.size);
            gnufilenames = std::string(b, entryinfo.size);
            readHeader();
        } else if (b[1] == ' ') {
            input->skip(entryinfo.size);
            readHeader();
        } else {
            int p = atoi(b+1);
            const char* c = gnufilenames.c_str() + p;
            const char* e = strchr(c, '/');
            if (e) {
                entryinfo.filename = std::string(c, e-c);
            } else {
                entryinfo.filename = c;
            }
        }
    } else {
        entryinfo.filename = std::string(b, len);
    }
}
