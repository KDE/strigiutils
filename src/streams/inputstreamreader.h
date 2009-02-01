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
#ifndef STRIGI_INPUTSTREAMREADER_H
#define STRIGI_INPUTSTREAMREADER_H

#include <string>
#include <iconv.h>
#include <strigi/strigiconfig.h>
#include "bufferedstream.h"
#include "streambase.h"

namespace Strigi {

/**
 * @brief Adapter to convert an InputStream (a byte stream) into a Reader
 * (decoded Unicode character stream).
 *
 * For more information of conversions, see
 * http://www.gnu.org/software/libc/manual/html_node/iconv-Examples.html and
 * http://tangentsoft.net/mysql++/doc/userman/html/unicode.html
 */
class STREAMS_EXPORT InputStreamReader : public BufferedReader {
private:
    iconv_t converter;
    bool finishedDecoding;
    InputStream* input;
    int32_t charsLeft;

    StreamBuffer<char> charbuf;
    void readFromStream();
    int32_t decode(wchar_t* start, int32_t space);
    int32_t fillBuffer(wchar_t* start, int32_t space);
public:
    /**
     * Create an InputStreamReader from the given InputStream.
     *
     * For a list of acceptable encodings, run <tt>iconv --list</tt>.
     * Not all encodings are supported on all systems.
     *
     * If the requested encoding is not available, the status is set to
     * Error and an error message is available by calling error().
     *
     * @param i the input stream to decode
     * @param enc the encoding of the input stream.  UTF-8 is assumed if
     * no encoding is given
     */
    explicit InputStreamReader(InputStream *i, const char *enc=0);
    ~InputStreamReader();
};

} // end namespace Strigi

#endif
