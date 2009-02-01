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
#ifndef STRIGI_STREAMENDANALYZER_H
#define STRIGI_STREAMENDANALYZER_H

#include <string>
#include "streamanalyzerfactory.h"

namespace Strigi {
class AnalysisResult;
template <class T> class StreamBase;
typedef StreamBase<char> InputStream;

class STREAMANALYZER_EXPORT StreamEndAnalyzer {
protected:
    std::string m_error;
public:
    virtual ~StreamEndAnalyzer() {}
    virtual bool checkHeader(const char* header, int32_t headersize) const = 0;
    virtual signed char analyze(Strigi::AnalysisResult& idx, InputStream* in)=0;
    const std::string error() const { return m_error; }
    virtual const char* name() const = 0;
};

class STREAMANALYZER_EXPORT StreamEndAnalyzerFactory
        : public StreamAnalyzerFactory {
public:
    virtual StreamEndAnalyzer* newInstance() const = 0;
    virtual bool analyzesSubStreams() const { return false; }
};


}

#endif
