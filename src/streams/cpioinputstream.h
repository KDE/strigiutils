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
#ifndef CPIOINPUTSTREAM_H
#define CPIOINPUTSTREAM_H

#include "substreamprovider.h"
#include "gzipinputstream.h"

namespace jstreams {

class CpioInputStream : public SubStreamProvider {
private:
    static const char unsigned* magic;
    char padding;

    void readHeader();
    static int32_t read4bytes(const unsigned char *b);
    int32_t readHexField(const char *b);
public:
    explicit CpioInputStream(StreamBase<char>* input);
    ~CpioInputStream();
    StreamBase<char>* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(StreamBase<char>* input) {
        return new CpioInputStream(input);
    }
};

} // end namespace jstreams

#endif
