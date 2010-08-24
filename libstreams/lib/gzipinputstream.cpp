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
#include <strigi/gzipinputstream.h>
#include <strigi/strigiconfig.h>
#include <zlib.h>
#include <stdlib.h>

using namespace Strigi;

class GZipInputStream::Private {
public:
    GZipInputStream* const p;
    InputStream* input;
    z_stream_s zstream;
    bool started;

    Private(GZipInputStream* gi, InputStream* input, ZipFormat format);
    ~Private();
    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
};
GZipInputStream::GZipInputStream(InputStream* input, ZipFormat format)
        :p(new Private(this, input, format)) {
}
GZipInputStream::Private::Private(GZipInputStream* gi,
        InputStream* i, ZipFormat format) :p(gi), input(i), started(false) {
    // initialize values that signal state
    p->m_status = Ok;

    // check first bytes of stream before allocating buffer
    if (format == GZIPFORMAT && !checkMagic()) {
        p->m_error = "Magic bytes for gz are wrong.";
        p->m_status = Error;
        return;
    }

    // initialize the z_stream
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.avail_in = 0;
    zstream.next_in = Z_NULL;
    // initialize for reading gzip streams
    // for reading libz streams, you need inflateInit(zstream)
    int r;
    switch(format) {
    case ZLIBFORMAT:
        r = inflateInit(&zstream);
        break;
    case GZIPFORMAT:
        r = inflateInit2(&zstream, 15+16);
        break;
    case ZIPFORMAT:
    default:
        r = inflateInit2(&zstream, -MAX_WBITS);
        break;
    }
    started = true;
    if (r != Z_OK) {
        p->m_error = "Error initializing GZipInputStream.";
        dealloc();
        p->m_status = Error;
        return;
    }

    // signal that we need to read into the buffer
    zstream.avail_out = 1;

    // initialize the buffer
    p->setMinBufSize(262144);
}
GZipInputStream::~GZipInputStream() {
    delete p;
}
GZipInputStream::Private::~Private() {
    dealloc();
}
void
GZipInputStream::Private::dealloc() {
    if (started) {
      inflateEnd(&zstream);
      started = false;
    }
    memset( &zstream, 0, sizeof( z_stream_s ) );
    input = NULL;
}
bool
GZipInputStream::Private::checkMagic() {
    const unsigned char* buf;
    const char* begin;
    int32_t nread;

    int64_t pos = input->position();
    nread = input->read(begin, 2, 2);
    input->reset(pos);
    if (nread != 2) {
        return false;
    }

    buf = (const unsigned char*)begin;
    return buf[0] == 0x1f && buf[1] == 0x8b;
}
void
GZipInputStream::Private::readFromStream() {
    // read data from the input stream
    const char* inStart;
    int32_t nread;
    nread = input->read(inStart, 1, 0);
    if (nread < -1) {
        p->m_status = Error;
        p->m_error = input->error();
    } else if (nread < 1) {
        p->m_status = Error;
        p->m_error.assign("unexpected end of stream");
    } else {
        zstream.next_in = (Bytef*)inStart;
        zstream.avail_in = nread;
    }
}
int32_t
GZipInputStream::fillBuffer(char* start, int32_t space) {
    if (p->input == NULL) return -1;
    z_stream_s& zstream = p->zstream;
    // make sure there is data to decompress
    if (zstream.avail_out) {
        p->readFromStream();
        if (m_status == Error) {
            // no data was read
            return -1;
        }
    }
    // make sure we can write into the buffer
    zstream.avail_out = space;
    zstream.next_out = (Bytef*)start;
    // decompress
    int r = inflate(&zstream, Z_SYNC_FLUSH);
    // inform the buffer of the number of bytes that was read
    int32_t nwritten = space - zstream.avail_out;
    switch (r) {
    case Z_NEED_DICT:
        m_error.assign("Z_NEED_DICT while inflating stream.");
        m_status = Error;
        break;
    case Z_DATA_ERROR:
        m_error.assign("Z_DATA_ERROR while inflating stream.");
        m_status = Error;
        break;
    case Z_MEM_ERROR:
        m_error.assign("Z_MEM_ERROR while inflating stream.");
        m_status = Error;
        break;
    case Z_STREAM_END:
        if (zstream.avail_in) {
            p->input->reset(p->input->position()-zstream.avail_in);
        }
        // we are finished decompressing,
        // (but this stream is not yet finished)
        p->dealloc();
    }
    return nwritten;
}
