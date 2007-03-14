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
#ifndef STRINGTERMINATEDSUBSTREAM
#define STRINGTERMINATEDSUBSTREAM

#include "streams_export.h"
#include "streambase.h"
#include "kmpsearcher.h"

namespace jstreams {

class STREAMS_EXPORT StringTerminatedSubStream : public StreamBase<char> {
private:
    const int64_t offset;
    StreamBase<char>* input;
    KmpSearcher searcher;
public:
    StringTerminatedSubStream(StreamBase<char>* i, const std::string& terminator)
        : offset(i->getPosition()), input(i) {
        searcher.setQuery(terminator);
    }
    int32_t read(const char*& start, int32_t min=0, int32_t max=0);
    int64_t reset(int64_t);
    int64_t getOffset() const { return offset; }
};

}

#endif
