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
#include "subinputstream.h"

#include "dostime.h"

using namespace jstreams;

/**
 * RPM files as specified here:
 * http://www.freestandards.org/spec/refspecs/LSB_1.3.0/gLSB/gLSB/swinstall.html
 **/
bool
RpmInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0xed,0xab,0xee,0xdb,0x03,0x00};
    if (datasize < 6) return false;
    // this check could be more strict
    bool ok = memcmp(data, magic, 6) == 0;
    return ok;
}
RpmInputStream::RpmInputStream(StreamBase<char>* input)
        : SubStreamProvider(input) {
    compressedEntryStream = 0;
    uncompressionStream = 0;
    uncompressedEntryStream = 0;
    // skip the header
    const char* b;
    // lead:96 bytes
    if (input->read(b, 96, 96) != 96) {
        status = Error;
        return;
    }
    printf("name: '%s'", b+10);
    for (int i=0; i<2; i++) {
        if (input->read(b, 16, 16) != 16) {
            status = Error;
            return;
        }
        int32_t nindex = read4bytes((const unsigned char*)(b+8));
        int32_t hsize = read4bytes((const unsigned char*)(b+12));
        int32_t sz = nindex*16+hsize;
        if (sz%8) sz+=8-sz%8;
        printf("%i %i %i %i\n", i, nindex, hsize, sz);
        input->skip(sz);
    }
    printf("%lli\n", input->getPosition());
    GZipInputStream z(input);
    while (!z.getStatus()) z.read(b, 1, 1);
    printf("%s\n", z.getError());
    status = Eof;
}
RpmInputStream::~RpmInputStream() {
    if (compressedEntryStream) {
        delete compressedEntryStream;
    }
    if (uncompressionStream) {
        delete uncompressionStream;
    }
    if (uncompressedEntryStream) {
        delete uncompressedEntryStream;
    }
}
StreamBase<char>*
RpmInputStream::nextEntry() {
    if (status) return 0;
    // clean up the last stream(s)
    if (uncompressedEntryStream) {
        if (compressedEntryStream) {
            compressedEntryStream->skip(compressedEntryStream->getSize());
            delete compressedEntryStream;
            compressedEntryStream = 0;
            delete uncompressionStream;
            uncompressionStream = 0;
        } else {
            uncompressedEntryStream->skip(uncompressedEntryStream->getSize());
        }
        delete uncompressedEntryStream;
        uncompressedEntryStream = 0;
    }
    readHeader();
    if (status) return 0;
    if (compressionMethod == 8) {
        compressedEntryStream = new SubInputStream(input, entryCompressedSize);
        if (uncompressionStream) {
            delete uncompressionStream;
        }
        uncompressionStream = new GZipInputStream(compressedEntryStream,
                GZipInputStream::ZIPFORMAT);
        uncompressedEntryStream
            = new SubInputStream(uncompressionStream, entryinfo.size);
    } else {
        uncompressedEntryStream = new SubInputStream(input, entryinfo.size);
    }
    return uncompressedEntryStream;
}
int32_t
RpmInputStream::read2bytes(const unsigned char *b) {
    return (b[0]<<8) + b[1]<<8;
}
int32_t
RpmInputStream::read4bytes(const unsigned char *b) {
    return (b[0]<<24) + (b[1]<<16) + (b[2]<<8) + b[3];
}
void
RpmInputStream::readHeader() {
    const unsigned char *hb;
    const char *b;
    int32_t toread;
    int32_t nread;

    // read the first 30 characters
    toread = 30;
    nread = input->read(b, toread, toread);
    if (nread != toread) {
        error = "Error reading zip header: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        status = Error;
        printf("%s\n", error.c_str());
        return;
    }
    hb = (const unsigned char*)b;
    // check the signature
    // check the first half of the signature
    if (hb[0] != 0x50 || hb[1] != 0x4b) {
        // signature is invalid
        status = Error;
        error = "Error: wrong zip signature.";
        return;
    }
    // check the second half of the signature
    if (hb[2] != 0x03 || hb[3] != 0x04) {
        // this may be the regular end of the file
        if (hb[2] != 0x01 || hb[3] != 0x02) {
            printf("This is new: %x %x %x %x\n", hb[0], hb[1], hb[2], hb[3]);
        }
        status = Eof;
        return;
    }
    // read 2 bytes into the filename size
    int32_t filenamelen = read2bytes(hb + 26);
    int64_t extralen = read2bytes(hb + 28);
    // read 4 bytes into the length of the uncompressed size
    entryinfo.size = read4bytes(hb + 22);
    // read 4 bytes into the length of the compressed size
    entryCompressedSize = read4bytes(hb + 18);
    compressionMethod = read2bytes(hb + 8);
    int32_t generalBitFlags = read2bytes(hb+6);
    if (generalBitFlags & 8) { // is bit 3 set?
        // ohoh, the file size and compressed file size are unknown at this
        // point in theory this is a solvable problem, but it's not easy:
        // one would need to keep a running crc32 and file size and match it
        // to the data read so far
        status = Error;
        error = "This particular zip file format is not supported for reading "
            "as a stream.";
        return;
    }
    unsigned long dost = read4bytes(hb+10);
    entryinfo.mtime = dos2unixtime(dost);

    readFileName(filenamelen);
    if (status) {
        status = Error;
        error = "Error reading file name.";
        return;
    }
    // read 2 bytes into the length of the extra field
    int64_t skipped = input->skip(extralen);
    if (skipped != extralen) {
        status = Error;
        error = "Error skipping extra field.";
        return;
    }
}
void
RpmInputStream::readFileName(int32_t len) {
    entryinfo.filename.resize(0);
    const char *begin;
    int32_t nread = input->read(begin, len, len);
    if (nread != len) {
        error = "Error reading filename: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        return;
    }
    entryinfo.filename.append(begin, nread);

    // temporary hack for determining if this is a directory:
    // does the filename end in '/'?
    len = entryinfo.filename.length();
    if (entryinfo.filename[len-1] == '/') {
        entryinfo.filename.resize(len-1);
        entryinfo.type = EntryInfo::Dir;
    } else {
        entryinfo.type = EntryInfo::File;
    }
}
