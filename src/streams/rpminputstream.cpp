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
#include "cpioinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include "subinputstream.h"

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
    cpio = 0;

    // skip the header
    const char* b;
    // lead:96 bytes
    if (input->read(b, 96, 96) != 96) {
        status = Error;
        error = "File is too small to be an RPM file.";
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
        error = uncompressionStream->getError();
        status = Error;
        return;
    }
    cpio = new CpioInputStream(uncompressionStream);
    status = cpio->getStatus();
}
RpmInputStream::~RpmInputStream() {
    if (uncompressionStream) {
        delete uncompressionStream;
    }
    if (cpio) {
        delete cpio;
    }
    if (headerinfo) {
        delete headerinfo;
    }
}
StreamBase<char>*
RpmInputStream::nextEntry() {
    if (status) return 0;
    StreamBase<char>* entry = cpio->nextEntry();
    status = cpio->getStatus();
    if (status == Error) {
        error = cpio->getError();
    }
    return entry;
}
int32_t
RpmInputStream::read4bytes(const unsigned char *b) {
    return (b[0]<<24) + (b[1]<<16) + (b[2]<<8) + b[3];
}
