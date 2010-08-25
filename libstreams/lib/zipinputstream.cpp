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

#include <strigi/zipinputstream.h>
#include <strigi/strigiconfig.h>
#include <strigi/gzipinputstream.h>
#include <strigi/subinputstream.h>

#include "dostime.h"
#include <strigi/textutils.h>
#include <cstring>


using namespace Strigi;

bool
ZipInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x50, 0x4b, 0x03, 0x04};
    if (datasize < 4) return false;
    bool ok = std::memcmp(data, magic, 4) == 0 && datasize > 8;
    return ok;
}
ZipInputStream::ZipInputStream(InputStream* input)
        : SubStreamProvider(input) {
    compressedEntryStream = 0;
    uncompressionStream = 0;
}
ZipInputStream::~ZipInputStream() {
    if (compressedEntryStream) {
        delete compressedEntryStream;
    }
    if (uncompressionStream) {
        delete uncompressionStream;
    }
}
InputStream*
ZipInputStream::nextEntry() {
    if (m_status) return NULL;
    // clean up the last stream(s)
    if (m_entrystream) {
	// if this entry is a compressed entry of know size, we can skip to
	// the end by skipping in the compressed stream, without decompressing
        if (compressedEntryStream) {
            compressedEntryStream->skip(compressedEntryStream->size());
            delete compressedEntryStream;
            compressedEntryStream = 0;
            delete uncompressionStream;
            uncompressionStream = 0;

            // check for a potential signature and skip it if it is there
            const char* c;
            int64_t p = m_input->position();
            int32_t n = m_input->read(c, 16, 16);
            if (n == 16) {
                n = readLittleEndianUInt32((const unsigned char*)c);
                if (n != 0x08074b50) {
                    m_input->reset(p);
                }
            }
        } else {
            int64_t size = m_entrystream->size();
            if (size < 1) {
                size = 1024;
            }
            while (m_entrystream->status() == Ok) {
                m_entrystream->skip(size);
            }
            if (m_entryinfo.size < 0) {
                // skip the data descriptor that occurs after the data
                const char* c;
                int32_t n = m_input->read(c, 4, 4);
                if (n == 4) {
                    n = readLittleEndianUInt32((const unsigned char*)c);
                    if (n == 0x08074b50) { // sometimes this signature appears
                        n = m_input->read(c, 12, 12);
                        n -= 8;
                    } else {
                        n = m_input->read(c, 8, 8);
                        n -= 4;
                    }
                }
                if (n != 4) {
                    m_status = Error;
                    m_error = "No valid data descriptor after entry data.";
                    return 0;
                }
            }
        }
        delete m_entrystream;
        m_entrystream = 0;
    }
    // are we at the end of the zip file?
    if (m_input->status() == Eof) {
        m_status = Eof;
        return NULL;
    }
    readHeader();
    if (m_status != Ok) return NULL;
    if (m_entryinfo.filename.length()<=0) {
        m_status = Error;
        m_error = "Archived file name is empty";
        return NULL;
    }    
    if (compressionMethod == 8) {
        if (m_entryinfo.size >= 0) {
            compressedEntryStream
                = new SubInputStream(m_input, entryCompressedSize);
            if (uncompressionStream) {
                delete uncompressionStream;
            }
            uncompressionStream = new GZipInputStream(compressedEntryStream,
                GZipInputStream::ZIPFORMAT);
            m_entrystream
                = new SubInputStream(uncompressionStream, m_entryinfo.size);
        } else {
            m_entrystream = new GZipInputStream(m_input,
                GZipInputStream::ZIPFORMAT);
        }
    } else {
        m_entrystream = new SubInputStream(m_input, m_entryinfo.size);
    }
    return m_entrystream;
}
void
ZipInputStream::readHeader() {
    const unsigned char *hb;
    const char *b;
    int32_t toread;
    int32_t nread;

    // read the first 30 characters
    toread = 30;
    nread = m_input->read(b, toread, toread);
    if (nread != toread) {
        m_error = "Error reading zip header: ";
        if (nread == -1) {
            m_error += m_input->error();
        } else {
            m_error += " premature end of file.";
        }
        m_status = Error;
        fprintf(stderr, "%s\n", m_error.c_str());
        return;
    }
    hb = (const unsigned char*)b;
    // check the signature
    // check the first half of the signature
    if (hb[0] != 0x50 || hb[1] != 0x4b) {
        // signature is invalid
        m_status = Error;
        m_error = "Error: wrong zip signature.";
        return;
    }
    // check the second half of the signature
    if (hb[2] != 0x03 || hb[3] != 0x04) {
        // this may be the start of the central file header
        if (hb[2] != 0x01 || hb[3] != 0x02) {
            fprintf(stderr, "This code in a zip file is strange: %x %x %x %x\n",
                hb[0], hb[1], hb[2], hb[3]);
        }
        m_status = Eof;
        return;
    }
    // read 2 bytes into the filename size
    int32_t filenamelen = readLittleEndianUInt16(hb + 26);
    int64_t extralen = readLittleEndianUInt16(hb + 28);
    // read 4 bytes into the length of the uncompressed size
    m_entryinfo.size = readLittleEndianUInt32(hb + 22);
    // read 4 bytes into the length of the compressed size
    entryCompressedSize = readLittleEndianUInt32(hb + 18);
    if (entryCompressedSize < 0) {
        m_status = Error;
        m_error = "Corrupt zip file with negative compressed size.";
        return;
    }
    compressionMethod = readLittleEndianUInt16(hb + 8);
    int32_t generalBitFlags = readLittleEndianUInt16(hb+6);
    if (generalBitFlags & 8) { // is bit 3 set?
        // ohoh, the file size and compressed file size are unknown at this
        // point
	// if the file is compressed with method 8 we rely on the decompression
	// stream to signal the end of the stream properly
        if (compressionMethod != 8) {
            m_status = Error;
            m_error = "This particular zip file format is not supported for "
                "reading as a stream.";
            return;
        }
        m_entryinfo.size = -1;
        entryCompressedSize = -1;
    }
    unsigned long dost = readLittleEndianUInt32(hb+10);
    m_entryinfo.mtime = dos2unixtime(dost);

    readFileName(filenamelen);
    if (m_status) {
        m_status = Error;
        m_error = "Error reading file name: ";
        m_error += m_input->error();
        return;
    }
    // read 2 bytes into the length of the extra field
    int64_t skipped = m_input->skip(extralen);
    if (skipped != extralen) {
        m_status = Error;
//	printf("skipped %li extralen %li position: %li size: %li\n", skipped, extralen, m_input->position(), m_input->size());
        m_error = "Error skipping extra field: ";
        m_error += m_input->error();
        return;
    }
}
void
ZipInputStream::readFileName(int32_t len) {
    m_entryinfo.filename.resize(0);
    const char *begin;
    int32_t nread = m_input->read(begin, len, len);
    if (nread != len) {
        m_error = "Error reading filename: ";
        if (nread == -1) {
            m_error += m_input->error();
        } else {
            m_error += " premature end of file.";
        }
        return;
    }
    m_entryinfo.filename.assign(begin, nread);

    // temporary hack for determining if this is a directory:
    // does the filename end in '/'?
    len = (int32_t)m_entryinfo.filename.length();
    if (m_entryinfo.filename[len-1] == '/') {
        m_entryinfo.filename.resize(len-1);
        m_entryinfo.type = EntryInfo::Dir;
    } else {
        m_entryinfo.type = EntryInfo::File;
    }
}
