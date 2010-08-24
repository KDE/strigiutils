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
#include <strigi/rpminputstream.h>
#include <strigi/cpioinputstream.h>
#include <strigi/gzipinputstream.h>
#include <strigi/lzmainputstream.h>
#include <strigi/bz2inputstream.h>
#include <strigi/subinputstream.h>
#include <strigi/textutils.h>

#include <list>

using namespace std;
using namespace Strigi;

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
RpmInputStream::RpmInputStream(InputStream* input)
        : SubStreamProvider(input), headerinfo(0) {
    uncompressionStream = NULL;
    cpio = 0;
    m_status = Error;

    // skip the header
    const char* b;
    // lead:96 bytes
    if (m_input->read(b, 96, 96) != 96) {
        m_error = "File is too small to be an RPM file.";
        return;
    }

    // read signature
    const unsigned char headmagic[] = {0x8e,0xad,0xe8,0x01};
    if (m_input->read(b, 16, 16) != 16 || memcmp(b, headmagic, 4)!=0) {
        m_error = "m_error in signature\n";
        return;
    }
    int32_t nindex = readBigEndianInt32(b+8);
    int32_t hsize = readBigEndianInt32(b+12);
    int32_t sz = nindex*16+hsize;
    if (sz%8) {
        sz+=8-sz%8;
    }
    if (m_input->read(b, sz, sz) != sz) {
        m_error = "RPM seems to be truncated or corrupted.";
        return;
    }

    // read header
    if (m_input->read(b, 16, 16) != 16 || memcmp(b, headmagic, 4)!=0) {
        m_error = "m_error in header\n";
        return;
    }
    nindex = readBigEndianInt32(b+8);
    hsize = readBigEndianInt32(b+12);
    int32_t size = nindex*16+hsize;
    if (m_input->read(b, size, size) != size) {
        m_error = "could not read header\n";
        return;
    }
    for (int32_t i=0; i<nindex; ++i) {
        const unsigned char* e = (const unsigned char*)b+i*16;
        /*int32_t tag =*/ readBigEndianInt32(e);
        /*int32_t type =*/ readBigEndianInt32(e+4);
        int32_t offset = readBigEndianInt32(e+8);
        if (offset < 0 || offset >= hsize) {
            m_error = "invalid offset in header\n";
            return;
        }
        /*int32_t count =*/ readBigEndianInt32(e+12);
        int32_t end = hsize;
        if (i < nindex-1) {
            end = readBigEndianInt32(e+8+16);
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

    int64_t pos = m_input->position();
    if (m_input->read(b, 16, 16) != 16) {
        m_error =  "could not read payload\n";
        return;
    }
    m_input->reset(pos);
    if (BZ2InputStream::checkHeader(b, 16)) {
        uncompressionStream = new BZ2InputStream(m_input);
    } else if (LZMAInputStream::checkHeader(b, 16)) {
        uncompressionStream = new LZMAInputStream(m_input);
    } else if (GZipInputStream::checkHeader(b, 16)) {
        uncompressionStream = new GZipInputStream(m_input);
    } else {
        m_error = "Unknown compressed stream type";
        return;
    };
    
    if (uncompressionStream->status() == Error) {
        m_error = uncompressionStream->error();
        return;
    }
    cpio = new CpioInputStream(uncompressionStream);
    m_status = cpio->status();
}
RpmInputStream::~RpmInputStream() {
    delete uncompressionStream;
    delete cpio;
    delete headerinfo;
    m_entrystream = 0;
}
InputStream*
RpmInputStream::nextEntry() {
    if (m_status) return 0;
    m_entrystream = cpio->nextEntry();
    m_status = cpio->status();
    if (m_status == Ok) {
        m_entryinfo = cpio->entryInfo();
    } else if (m_status == Error) {
        m_error = cpio->error();
    }
    return m_entrystream;
}
