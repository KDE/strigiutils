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
#ifndef GZIPINPUTSTREAM_H
#define GZIPINPUTSTREAM_H

#include "jstreamsconfig.h"
#include "streambase.h"
#include "bufferedstream.h"

struct z_stream_s;

namespace Strigi {

class STREAMS_EXPORT GZipInputStream : public BufferedInputStream {
private:
    z_stream_s* zstream;
    InputStream* input;

    void dealloc();
    void readFromStream();
    void decompressFromStream();
    bool checkMagic();
public:
    enum ZipFormat { ZLIBFORMAT, GZIPFORMAT, ZIPFORMAT};
    explicit GZipInputStream(InputStream* input,
        ZipFormat format=GZIPFORMAT);
    ~GZipInputStream();
    int32_t fillBuffer(char* start, int32_t space);
};

} // end namespace Strigi

#endif
