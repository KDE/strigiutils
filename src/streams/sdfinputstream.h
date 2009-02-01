/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#ifndef STRIGI_SDFINPUTSTREAM_H
#define STRIGI_SDFINPUTSTREAM_H

#include "substreamprovider.h"
#include "subinputstream.h"
#include "kmpsearcher.h"

namespace Strigi {

// specification can be found here:
// http://www.symyx.com/downloads/public/ctfile/ctfile.jsp

class STREAMS_EXPORT SdfInputStream : public SubStreamProvider {
private:
    SubInputStream* substream;
    int32_t entrynumber;
    int64_t previousStartOfDelimiter;
    KmpSearcher m_searcher;
    static const std::string delimiter;
    static const std::string label;
public:
    explicit SdfInputStream(InputStream* input);
    ~SdfInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new SdfInputStream(input);
    }
};

} // end namespace Strigi

#endif
