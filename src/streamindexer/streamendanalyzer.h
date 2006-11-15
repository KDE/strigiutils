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
#ifndef STREAMENDANALYZER_H
#define STREAMENDANALYZER_H

#include "inputstream.h"

namespace jstreams {
class StreamIndexer;
class Indexable;

class StreamEndAnalyzer {
protected:
    std::string error;
    static char testStream(jstreams::InputStream *in);
public:
    virtual ~StreamEndAnalyzer() {};
    virtual bool checkHeader(const char* header, int32_t headersize) const = 0;
    virtual char analyze(std::string filename, jstreams::InputStream* in,
        int depth, StreamIndexer* indexer, jstreams::Indexable*) = 0;
    const std::string& getError() const { return error; }
    virtual const char* getName() const = 0;
};

}

#endif
