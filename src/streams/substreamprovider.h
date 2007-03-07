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
#ifndef SUBSTREAMPROVIDER
#define SUBSTREAMPROVIDER

#include <string>
#include "streams_export.h"
#include "streambase.h"

namespace jstreams {

struct EntryInfo {
    std::string filename;
    int32_t size;
    uint mtime;
    enum Type {Unknown=0, Dir=1, File=2};
    Type type;
    EntryInfo() :size(-1), mtime(0), type(Unknown) {}
};

class STREAMS_EXPORT SubStreamProvider {
protected:
    StreamStatus status;
    std::string error;
    StreamBase<char> *input;
    StreamBase<char> *entrystream;
    EntryInfo entryinfo;
public:
    SubStreamProvider(StreamBase<char> *i) :status(Ok), input(i), entrystream(0)
        {}
    virtual ~SubStreamProvider() { if (entrystream) delete entrystream; }
    StreamStatus getStatus() const { return status; }
    virtual StreamBase<char>* nextEntry() = 0;
    StreamBase<char>* currentEntry() { return entrystream; }
    const EntryInfo &getEntryInfo() const {
        return entryinfo;
    }
    const char* getError() const { return error.c_str(); }
};

} // end namespace jstreams

#endif
