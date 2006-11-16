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
#ifndef STREAMINDEXER_H
#define STREAMINDEXER_H

#include <vector>
#include "moduleloader.h"

namespace jstreams {
class StreamEndAnalyzer;
class StreamThroughAnalyzer;
class IndexWriter;
class Indexable;
template <class T> class StreamBase;
/**
 * The class StreamIndexer extracts information from files or character
 * streams. This task is performed by two separate analyzer classes:
 * - EndAnalyzer     pulls the data through the stream
 * - ThroughAnalyzer analyzes the data as it comes along
 * Examples for ThroughAnalyzers are classes that calculate a hash or
 * a checksum on a stream, or determine character usage.
 * An endanalyzer can do different things depending on the file type.
 * It could filter words from a text or split a stream up in substreams.
 * Especially the last task is important when working with archive streams
 * such as zip or tar files that contain other files.
 *
 * Currently this class is very simple and mainly useful for calculating
 * digests of streams in files in nested archives.
 **/
class StreamIndexer {
private:
    IndexWriter* writer;
    std::vector<std::vector<jstreams::StreamEndAnalyzer*> > end;
    std::vector<std::vector<jstreams::StreamThroughAnalyzer*> > through;

    ModuleLoader moduleLoader;
    void addThroughAnalyzers();
    void addEndAnalyzers();
    void removeIndexable(uint depth);
public:
    explicit StreamIndexer(IndexWriter *w);
    ~StreamIndexer();
    char indexFile(const char *filepath);
    char indexFile(const std::string& filepath);
    char analyze(Indexable& idx, jstreams::StreamBase<char> *input);
};
}

#endif
