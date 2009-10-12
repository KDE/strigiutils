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
#ifndef STRIGI_STREAMTHROUGHANALYZER_H
#define STRIGI_STREAMTHROUGHANALYZER_H

#include "streamanalyzerfactory.h"

namespace Strigi {

class AnalysisResult;
template <class T> class StreamBase;
typedef StreamBase<char> InputStream;

/**
 * This class defines an interface for analyzing streams.
 * This interface is not yet complete, it lacks good functions for retrieving
 * the results in other ways than just printing them.
 */
class STREAMANALYZER_EXPORT StreamThroughAnalyzer {
public:
    virtual ~StreamThroughAnalyzer() {}
    /**
     * Passes a pointer to the AnalysisResult to be used for
     * this stream.
     **/
    virtual void setIndexable(Strigi::AnalysisResult*) = 0;
    /**
     * This function sets up the analyzer for handling a stream.
     * The stream \p in is used in constructing a new internal
     * inputstream that is returned by this function. Every time
     * a read call is performed on this class, the data read
     * is used in the analysis before being passed on to the
     * caller. The InputStream pointer maybe 0 for special filetypes that have
     * no content, so any implementation must always check the pointer before
     * reading from the stream.
     * The stream @p in should be returned and it should be at position 0.
     * This means reading from the stream is only allowed when the stream is reset to position 0.
     **/
    virtual InputStream *connectInputStream(InputStream *in) = 0;
    /**
     * Signals to the caller whether this analyzer is finished
     * with the current stream.
     **/
    virtual bool isReadyWithStream() = 0;
    /**
     * Return the name of this throughanalyzer.
     **/
    virtual const char* name() const = 0;
};

/**
 * The factory class for the StreamThroughAnalyzer
 */
class STREAMANALYZER_EXPORT StreamThroughAnalyzerFactory
        : public StreamAnalyzerFactory {
public:
    virtual StreamThroughAnalyzer* newInstance() const = 0;
};

} // end namespace Strigi

#endif
