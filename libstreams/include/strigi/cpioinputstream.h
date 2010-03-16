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
#ifndef STRIGI_CPIOINPUTSTREAM_H
#define STRIGI_CPIOINPUTSTREAM_H

#include <strigi/substreamprovider.h>
#include <strigi/gzipinputstream.h>

namespace Strigi {

class STREAMS_EXPORT CpioInputStream : public SubStreamProvider {
private:
    static const char unsigned* magic;
    char padding;

    void readHeader();
    int32_t readHexField(const char *b);
public:
    explicit CpioInputStream(InputStream* input);
    ~CpioInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new CpioInputStream(input);
    }
};

} // end namespace Strigi

#endif
