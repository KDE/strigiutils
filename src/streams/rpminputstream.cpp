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
#include "rpminputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include "subinputstream.h"

#include "dostime.h"

#include <list>

using namespace jstreams;
using namespace std;

class RpmInputStream::RpmHeaderInfo {
};
/**
 * RPM files as specified here:
 * http://www.freestandards.org/spec/refspecs/LSB_1.3.0/gLSB/gLSB/swinstall.html
 * and here:
 * http://www.rpm.org/max-rpm-snapshot/s1-rpm-file-format-rpm-file-format.html
 **/
bool
RpmInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char unsigned magic[] = {0xed,0xab,0xee,0xdb,0x03,0x00};
    if (datasize < 6) return false;
    // this check could be more strict
    bool ok = memcmp(data, magic, 6) == 0;
    return ok;
}
RpmInputStream::RpmInputStream(StreamBase<char>* input)
        : SubStreamProvider(input), headerinfo(0) {
    uncompressionStream = 0;

    // skip the header
    const char* b;
    // lead:96 bytes
    if (input->read(b, 96, 96) != 96) {
        status = Error;
        return;
    }

    // read signature
    const unsigned char headmagic[] = {0x8e,0xad,0xe8,0x01};
    if (input->read(b, 16, 16) != 16 || memcmp(b, headmagic, 4)!=0) {
        error = "error in signature\n";
        status = Error;
        return;
    }
    int32_t nindex = read4bytes((const unsigned char*)(b+8));
    int32_t hsize = read4bytes((const unsigned char*)(b+12));
    int32_t sz = nindex*16+hsize;
    if (sz%8) {
        sz+=8-sz%8;
    }
    input->read(b, sz, sz);

    // read header
    if (input->read(b, 16, 16) != 16 || memcmp(b, headmagic, 4)!=0) {
        error = "error in header\n";
        status = Error;
        return;
    }
    nindex = read4bytes((const unsigned char*)(b+8));
    hsize = read4bytes((const unsigned char*)(b+12));
    int32_t size = nindex*16+hsize;
    if (input->read(b, size, size) != size) {
        error = "could not read header\n";
        status = Error;
        return;
    }
    for (int32_t i=0; i<nindex; ++i) {
        const unsigned char* e = (const unsigned char*)b+i*16;
        int32_t tag = read4bytes(e);
        int32_t type = read4bytes(e+4);
        int32_t offset = read4bytes(e+8);
        if (offset < 0 || offset >= hsize) {
            // error: invalid offset
        }
        int32_t count = read4bytes(e+12);
        int32_t end = hsize;
        if (i < nindex-1) {
            end = read4bytes(e+8+16);
        }
        if (end < offset) end = offset;
        if (end > hsize) end = hsize;
// TODO actually put the data into the objects so the analyzers can use them
/*        if (type == 6) {
            string s(b+nindex*16+offset, end-offset);
            printf("%s\n", s.c_str());
        } else if (type == 8 || type == 9) {
            list<string> v;
            // TODO handle string arrays
        }
        printf("%i\t%i\t%i\t%i\n", tag, type, offset, count);*/
    }

    int64_t pos = input->getPosition();
    if (input->read(b, 16, 16) != 16) {
        error =  "could not read payload\n";
        status = Error;
        return;
    }
    input->reset(pos);
    if (BZ2InputStream::checkHeader(b, 16)) {
        uncompressionStream = new BZ2InputStream(input);
    } else {
        uncompressionStream = new GZipInputStream(input);
    }
    if (uncompressionStream->getStatus() == Error) {
        status = Error;
        return;
    }
}
RpmInputStream::~RpmInputStream() {
    if (uncompressionStream) {
        delete uncompressionStream;
    }
    if (headerinfo) {
        delete headerinfo;
    }
}
StreamBase<char>*
RpmInputStream::nextEntry() {
    if (status) return 0;
    if (entrystream) {
        while (entrystream->getStatus() == Ok) {
            entrystream->skip(entrystream->getSize());
        }
        delete entrystream;
        entrystream = 0;
        if (padding) {
            uncompressionStream->skip(padding);
        }
    }
    readHeader();
    if (status) return 0;
    entrystream = new SubInputStream(uncompressionStream, entryinfo.size);
    return entrystream;
}
int32_t
RpmInputStream::read4bytes(const unsigned char *b) {
    return (b[0]<<24) + (b[1]<<16) + (b[2]<<8) + b[3];
}
void
RpmInputStream::readHeader() {
    //const unsigned char *hb;
    const char *b;
    int32_t toread;
    int32_t nread;

    // read the first 110 characters
    toread = 110;
    nread = uncompressionStream->read(b, toread, toread);
    if (nread != toread) {
        status = uncompressionStream->getStatus();
        if (status == Eof) {
            return;
        }
        error = "Error reading rpm entry: ";
        if (nread == -1) {
            error += uncompressionStream->getError();
        } else {
            error += " premature end of file.";
        }
        return;
    }
    // check header
    if (memcmp(b, "070701", 6) != 0) {
        status = Error;
        error = "RPM Entry signature is unknown: ";
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
    if (padding) padding = 4-padding;

    // read filename
    toread = filenamesize+namepadding;
    nread = uncompressionStream->read(b, toread, toread);
    if (nread != toread) {
        error = "Error reading rpm entry name.";
        status = Error;
        return;
    }
    int32_t len = filenamesize-1;
    if (len > 2 && b[0] == '.' && b[1] == '/') {
        b += 2;
    }
    entryinfo.filename = std::string((const char*)b, filenamesize-1);
}
int32_t
RpmInputStream::readHexField(const char *b) {
    int32_t val = 0;
    char c;
    for (char i=0; i<8; ++i) {
        val <<= 4;
        c = b[i];
        if (c > '9') {
            val += 10+c-'a';
        } else {
            val += c-'0';
        }
    }
    return val;
}
