/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias G. Pfeiffer <tgpfeiffer@web.de>
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
#ifndef STRIGI_STREAMLINEANALYZER_H
#define STRIGI_STREAMLINEANALYZER_H

#include "streamanalyzerfactory.h"

namespace Strigi {
class AnalysisResult;

/**
 * This class is especially well suited for file formats that are based on
 * lines of plain text, i.e. where a line break indicates information separation,
 * such as CSV as opposed to, for example, SVG files, where line breaks do not
 * necessarily indicate start/end of information blocks.
 * (However, this class can still be used for that purpose, it just isn't the
 * best choice.)
 */
class STREAMANALYZER_EXPORT StreamLineAnalyzer {
public:
    /**
     * Destructor. Clean up your room :-)
     */
    virtual ~StreamLineAnalyzer() {}
    /**
     * Returns the name of this analyzer. Taking the class name is fine
     * for this purpose.
     */
    virtual const char* name() const = 0;
    /**
     * Is called to signal the beginning of a stream analysis. This is the place
     * to initialize variables that need to be set again for every stream.
     * If there are single objects that will be needed again for every analyzed
     * stream, they should be created in the constructor.
     * \param result pointer to the AnalysisResult to write your results to
     */
    virtual void startAnalysis(AnalysisResult* result) = 0;
    /**
     * Is called when the analysis of a stream is finished. You can do
     * cleanups here, if necessary.
     * \param complete  This parameter tell whether all of the file was read.
     */
    virtual void endAnalysis(bool complete) = 0;
    /**
     * Is called for every line in the stream. Here, the actual analysis
     * takes place.
     * \param data character data of the line
     * \param length number of characters in that line
     */
    virtual void handleLine(const char* data, uint32_t length) = 0;
    /**
     * Tells the caller whether you are finished with your analysis or not.
     * If this returns true, this Analyzer will receive no more data from
     * the stream, i.e. handleLine() will not be called again.
     * If this analyzer notices it is not able to use the given data (for
     * example, this is a completely different file format), have this
     * function return true ASAP.
     * \return true if you are finished with this stream, false otherwise
     */
    virtual bool isReadyWithStream() = 0;
};

/**
 * This is the factory for the creation of a StreamLineAnalyzer.
 */
class STREAMANALYZER_EXPORT StreamLineAnalyzerFactory
        : public StreamAnalyzerFactory {
public:
    /**
     * Is called to create a new instance of the corresponding StreamLineAnalyzer.
     * \return pointer to the new analyzer instance
     */
    virtual StreamLineAnalyzer* newInstance() const = 0;
};

}

#endif
