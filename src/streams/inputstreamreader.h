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
#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include <string>
#include <iconv.h>
#include "streams_export.h"
#include "bufferedstream.h"

namespace jstreams {

/**
 * Info on conversions:
http://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html
http://tangentsoft.net/mysql++/doc/userman/html/unicode.html
 **/
class STREAMS_EXPORT InputStreamReader : public BufferedInputStream<wchar_t> {
private:
    iconv_t converter;
    bool finishedDecoding;
    StreamBase<char>* input;
    int32_t charsLeft;

    InputStreamBuffer<char> charbuf;
    void readFromStream();
    int32_t decode(wchar_t* start, int32_t space);
public:
    explicit InputStreamReader(StreamBase<char> *i, const char *enc=0);
    ~InputStreamReader();
    int32_t fillBuffer(wchar_t* start, int32_t space);
};

} // end namespace jstreams

#endif
