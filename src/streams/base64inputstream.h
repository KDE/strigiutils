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
#ifndef BASE64INPUTSTREAM_H
#define BASE64INPUTSTREAM_H

#include "bufferedstream.h"

namespace jstreams {
class Base64InputStream : public BufferedInputStream<char> {
private:
    int32_t bits;
    char bytestodo;
    char char_count;
    const char* pos, * pend;
    int32_t nleft;

    StreamBase<char>* input;

    static const unsigned char alphabet[];
    static bool inalphabet[256];
    static unsigned char decoder[133];
    static bool initializedAlphabet;
    static void initialize();

    bool moreData();
public:
    explicit Base64InputStream(StreamBase<char>* i);
    int32_t fillBuffer(char* start, int32_t space);
    static std::string decode(const char*c, uint32_t length);
};
}

#endif
