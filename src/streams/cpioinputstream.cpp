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
#include "cpioinputstream.h"
#include "subinputstream.h"

#include <list>

using namespace jstreams;
using namespace std;

const char unsigned* CpioInputStream::magic = (const char unsigned *)"070701";

bool
CpioInputStream::checkHeader(const char* data, int32_t datasize) {
    if (datasize < 6) return false;
    // this check could be more strict
    bool ok = memcmp(data, magic, 6) == 0;
    return ok;
}
CpioInputStream::CpioInputStream(StreamBase<char>* input)
        : SubStreamProvider(input) {
}
CpioInputStream::~CpioInputStream() {
}
StreamBase<char>*
CpioInputStream::nextEntry() {
    if (status) return 0;
    if (entrystream) {
        while (entrystream->getStatus() == Ok) {
            entrystream->skip(entrystream->getSize());
        }
        delete entrystream;
        entrystream = 0;
        if (padding) {
            input->skip(padding);
        }
    }
    readHeader();
    entrystream = new SubInputStream(input, entryinfo.size);
    return (status) ?0 :entrystream;
}
int32_t
CpioInputStream::read4bytes(const unsigned char *b) {
    return (b[0]<<24) + (b[1]<<16) + (b[2]<<8) + b[3];
}
void
CpioInputStream::readHeader() {
    //const unsigned char *hb;
    const char* b;
    int32_t toread;
    int32_t nread;

    // read the first 110 characters
    toread = 110;
    nread = input->read(b, toread, toread);
    if (nread != toread) {
        status = input->getStatus();
        if (status == Eof) {
            return;
        }
        error = "Error reading cpio entry: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        return;
    }
    // check header
    if (memcmp(b, magic, 6) != 0) {
        status = Error;
        error = "CPIO Entry signature is unknown: ";
        error.append(b, 6);
        return;
    }
    entryinfo.size = readHexField(b+54);
    entryinfo.mtime = readHexField(b+46);
    int32_t filenamesize = readHexField(b+94);
    if (status) {
        error = "Error parsing entry field.";
        return;
    }
    char namepadding = (filenamesize+2) % 4;
    if (namepadding) namepadding = 4 - namepadding;
    padding = entryinfo.size % 4;
    if (padding) padding = 4 - padding;

    // read filename
    toread = filenamesize + namepadding;
    nread = input->read(b, toread, toread);
    if (nread != toread) {
        error = "Error reading cpio entry name.";
        status = Error;
        return;
    }
    int32_t len = filenamesize - 1;
    if (len > 2 && b[0] == '.' && b[1] == '/') {
        b += 2;
    }
    // check if the name is not shorter than specified
    len = 0;
    while (len < filenamesize && b[len] != '\0') len++;
    entryinfo.filename = std::string((const char*)b, len);

    // if an cpio has an entry 'TRAILER!!!' we are at the end
    if ("TRAILER!!!" == entryinfo.filename) {
        status = Eof;
    }
}
int32_t
CpioInputStream::readHexField(const char *b) {
    int32_t val = 0;
    char c;
    for (unsigned char i=0; i<8; ++i) {
        val <<= 4;
        c = b[i];
        if (c > 'F') {
            val += c - 87;
        } else if (c > '9') {
            val += c - 55;
        } else {
            val += c - 48;
        }
    }
    return val;
}
