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
#ifndef STRIGI_STREAMANALYZER_H
#define STRIGI_STREAMANALYZER_H

#include <strigi/strigiconfig.h>
#include <string>


namespace Strigi {

template <class T> class StreamBase;
class AnalyzerConfiguration;
class IndexWriter;
class AnalysisResult;
class StreamAnalyzerPrivate;

/**
 * The class StreamAnalyzer extracts information from files or character
 * streams. This task is performed by two separate analyzer classes:
 * - EndAnalyzer     pulls the data through the stream
 * - ThroughAnalyzer analyzes the data as it comes along
 * Examples for ThroughAnalyzers are classes that calculate a hash or
 * a checksum on a stream, or determine character usage.
 * An endanalyzer can do different things depending on the file type.
 * It could filter words from a text or split a stream up in substreams.
 * Especially the last task is important when working with archive streams
 * such as zip or tar files that contain other files.
 **/
class StreamAnalyzerPrivate;
class STREAMANALYZER_EXPORT StreamAnalyzer {
private:
    StreamAnalyzerPrivate* const p;

public:
    StreamAnalyzer(AnalyzerConfiguration& c);
    ~StreamAnalyzer();
    void setIndexWriter(IndexWriter& writer);
    signed char indexFile(const char *filepath);
    signed char indexFile(const std::string& filepath);
    signed char analyze(AnalysisResult& idx, StreamBase<char> *input);
    AnalyzerConfiguration& configuration() const;
};
}

#endif
