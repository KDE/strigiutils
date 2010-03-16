/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
#include "gzipcompressstream.h"
#include <zlib.h>
#include <iostream>

using namespace Strigi;
using namespace std;

GZipCompressInputStream::GZipCompressInputStream(InputStream* input, int level) {
    // initialize values that signal state
    m_status = Ok;
    zstream = 0;
    if ( level < 0 || level > 9 ) {
        level = Z_DEFAULT_COMPRESSION;
    }

    this->input = input;

    // initialize the z_stream
    zstream = (z_stream_s*)malloc(sizeof(z_stream_s));
    zstream->zalloc = Z_NULL;
    zstream->zfree = Z_NULL;
    zstream->opaque = Z_NULL;
    zstream->avail_in = 0;

    // initialize for writing gzip streams
    int r = deflateInit(zstream, level);
    if (r != Z_OK) {
        m_error = "Error initializing GZipCompressInputStream.";
        dealloc();
        m_status = Error;
        return;
    }

    // signal that we need to read into the buffer
    zstream->avail_out = 1;
}
GZipCompressInputStream::~GZipCompressInputStream() {
    dealloc();
}
void
GZipCompressInputStream::dealloc() {
    if (zstream) {
        deflateEnd(zstream);
        free(zstream);
        zstream = 0;
    }
}
void
GZipCompressInputStream::readFromStream() {
    // read data from the input stream
    const char* inStart;
    int32_t nread;
    nread = input->read(inStart, 1, 0);
    if (nread < -1) {
        m_status = Error;
        m_error = input->error();
    } else if (nread < 1) {
        zstream->avail_in = 0; //bail...
    } else {
        zstream->next_in = (Bytef*)inStart;
        zstream->avail_in = nread;
    }
}
int32_t
GZipCompressInputStream::fillBuffer(char* start, int32_t space) {
    cerr << "GZCI " << this << " " << zstream << endl;
    if (zstream == 0) return -1;

    // make sure we can write into the buffer
    zstream->avail_out = space;
    zstream->next_out = (Bytef*)start;

    int r;
    // make sure there is data to decompress
    if (zstream->avail_in == 0) {
        readFromStream();
        if (m_status == Error) {
            cerr << "error " << endl;
            // no data was read
            return -1;
        } else if (zstream->avail_in == 0) {
            r = deflate(zstream, Z_FINISH);
            int32_t nwritten = space - zstream->avail_out;
            cerr << "GZCI end " << this << " " << nwritten << " " << m_status
<< endl;
            if (r != Z_OK) {
                cerr << "GZCI streamend " << r << endl;
                dealloc();
                if (r != Z_STREAM_END) {
                    fprintf(stderr, "deflate should report Z_STREAM_END\n");
                    return -1;
                }
            }
            return nwritten;
        }
    }
    r = deflate(zstream, Z_NO_FLUSH);
    // inform the buffer of the number of bytes that was read
    int32_t nwritten = space - zstream->avail_out;
    switch (r) {
    case Z_NEED_DICT:
        m_error = "Z_NEED_DICT while inflating stream.";
        m_status = Error;
        break;
    case Z_DATA_ERROR:
        m_error = "Z_DATA_ERROR while inflating stream.";
        m_status = Error;
        break;
    case Z_MEM_ERROR:
        m_error = "Z_MEM_ERROR while inflating stream.";
        m_status = Error;
        break;
    }
    cerr << "GZCI more " << this << " " << nwritten << endl;
    return nwritten;
}
