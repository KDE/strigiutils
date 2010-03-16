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
#ifndef STRIGI_TARINPUTSTREAM_H
#define STRIGI_TARINPUTSTREAM_H

#include <strigi/streambase.h>
#include <strigi/substreamprovider.h>

namespace Strigi {

/**
 * Implementation of SubStreamProvider for reading the tar format. The InputStream recognizes this format:
 * - tar archive is composed of blocks of 512 bytes. Every file and every header starts at an offset
 *   of a multiple of 512.
 * - each file starts with a header of size 512.
 * - up to 100 bytes of filename at offset 0 of file header
 * - file size is given in 7 bytes of octal format starting at position 124 of the file header
 * - if the filename is '././\@LongLink' then this block contains the length of the file name at
 *   position 124. The next blocks contain the filename. The block after that is the real header
 *   with the file size and only then does the file contents start.
 *
 * http://www.gnu.org/software/tar/manual/html_node/Standard.html
 **/
class STREAMS_EXPORT TarInputStream : public SubStreamProvider {
private:
    // information relating to the current entry
    int32_t numPaddingBytes;

    void readFileName(int32_t len);
    const char* readHeader();
    void parseHeader();
    int32_t readOctalField(const char *b, int32_t offset);
    void readLongLink(const char *b);
public:
    explicit TarInputStream(InputStream *input);
    ~TarInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new TarInputStream(input);
    }
};

} // end namespace Strigi

#endif
