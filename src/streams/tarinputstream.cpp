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
#include "tarinputstream.h"
#include "subinputstream.h"
#include <cstring>
using namespace jstreams;

TarInputStream::TarInputStream(StreamBase<char>* input)
        : SubStreamProvider(input) {
    output = 0;
}

TarInputStream::~TarInputStream() {
    if (output) {
        delete output;
    }
}
StreamBase<char>*
TarInputStream::nextEntry() {
    if (status) return 0;
    if (output) {
        output->skip(output->getSize());
        input->skip(numPaddingBytes);
        delete output;
        output = 0;
    }
    parseHeader();
    if (status) return 0;
    output = new SubInputStream(input, entryinfo.size);
    return output;
}
const char*
TarInputStream::readHeader() {
    // read the first 500 characters
    const char *begin;
    int32_t nread = input->read(begin, 512, 512);
    if (nread != 512) {
        status = Error;
    }
    return begin;
}
bool
TarInputStream::checkHeader(const char* h, const int32_t hsize) {
    if (hsize < 257) {
        // header is too small to check
        return false;
    }
    // check for field values that should be '\0' for the header to be a
    // tar header. Two positions are also accepted if they are ' ' because they
    return !(h[107] || h[115] || h[123] || (h[135]&&h[135]!=' ')
            || (h[147] && h[147] != ' ') || h[256]);
}
void
TarInputStream::parseHeader() {
    const char *hb;
    hb = readHeader();
    if (status) return;

    // check for terminators ('\0') on the first couple of fields
    if (!checkHeader(hb, 257)) {
        error = "Invalid tar header.\n";
        status = Error;
        return;
    }

    int32_t len = strlen(hb);
    if (len == 0) {
        // ready
        status = Eof;
        return;
    }
    if (len > 100) len = 100;
    entryinfo.filename.resize(0);
    entryinfo.filename.append(hb, len);
    if (entryinfo.filename == "././@LongLink") {
        entryinfo.filename.resize(0);
        readLongLink(hb);
        if (status) return;
        hb = readHeader();
        if (status) return;
    }

    // read the file size which is in octal format
    entryinfo.size = readOctalField(hb, 124);
    if (status) return;
    entryinfo.mtime = readOctalField(hb, 136);
    if (status) return;

    numPaddingBytes = 512 - entryinfo.size%512;
    if (numPaddingBytes == 512) {
        numPaddingBytes = 0;
    }

    len = entryinfo.filename.length();
    if (entryinfo.filename[len-1] == '/') {
        entryinfo.filename.resize(len-1);
    }
    // read file type
    if (hb[156] == 0 || hb[156] == '0') {
        entryinfo.type = EntryInfo::File;
    } else if (hb[156] == '5') {
        entryinfo.type = EntryInfo::Dir;
    } else {
        entryinfo.type = EntryInfo::Unknown;
    }
//    printf("!%s %i\n", entryinfo.filename.c_str(), hb[156]);
}
int32_t
TarInputStream::readOctalField(const char *b, int32_t offset) {
    int32_t val;
    int r = sscanf(b+offset, "%o", &val);
    if (r != 1) {
        status = Error;
        error = "Error reading header: octal field is not a valid integer.";
        return 0;
    }
    return val;
}
void
TarInputStream::readLongLink(const char *b) {
    int32_t toread = readOctalField(b, 124);
    int32_t left = toread%512;
    if (left) {
        left = 512 - left;
    }
    const char *begin;
    if (status) return;
    int32_t nread = input->read(begin, toread, toread);
    if (nread != toread) {
            status = Error;
            error = "Error reading LongLink: ";
            if (nread == -1) {
                error += input->getError();
            } else {
                error += " premature end of file.";
            }
            return;
    }
    entryinfo.filename.append(begin, nread);

    int64_t skipped = input->skip(left);
    if (skipped != left) {
        status = Error;
        error = "Error reading LongLink: ";
        if (input->getStatus() == Error) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
    }
}
