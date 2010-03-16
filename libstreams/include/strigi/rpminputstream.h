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
#ifndef STRIGI_RPMINPUTSTREAM_H
#define STRIGI_RPMINPUTSTREAM_H

#include <strigi/streambase.h>
#include <strigi/substreamprovider.h>

namespace Strigi {

class CpioInputStream;
class STREAMS_EXPORT RpmInputStream : public SubStreamProvider {
private:
    // information relating to the current entry
    InputStream* uncompressionStream;
    CpioInputStream* cpio;
    int32_t entryCompressedSize;
    int32_t compressionMethod;
    class RpmHeaderInfo;
    RpmHeaderInfo *headerinfo;

public:
    explicit RpmInputStream(InputStream* input);
    ~RpmInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new RpmInputStream(input);
    }
};

} // end namespace Strigi

#endif
