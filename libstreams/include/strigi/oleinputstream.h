/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_OLEINPUTSTREAM_H
#define STRIGI_OLEINPUTSTREAM_H

#include <strigi/strigiconfig.h>
#include <strigi/streambase.h>
#include <strigi/substreamprovider.h>
#include <vector>

namespace Strigi {

/**
 * Partial implementation of the ole file format according to
 * http://jakarta.apache.org/poi/poifs/fileformat.html
 **/
class STREAMS_EXPORT OleInputStream : public SubStreamProvider {
public:
    class Private;
private:
    Private* const p;
public:
    explicit OleInputStream(InputStream* input);
    ~OleInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new OleInputStream(input);
    }
};

} // end namespace Strigi

#endif
