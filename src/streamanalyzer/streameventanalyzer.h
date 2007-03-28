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
#ifndef STREAMSTREAMANALYZER_H
#define STREAMSTREAMANALYZER_H

#include "inputstream.h"

namespace Strigi {
class StreamAnalyzer;
class RegisteredField;
class FieldRegister;
class AnalysisResult;

class STREAMANALYZER_EXPORT StreamEventAnalyzer {
public:
    virtual ~StreamEventAnalyzer() {}
    virtual const char* getName() const = 0;
    virtual void startAnalysis(AnalysisResult*) = 0;
    virtual void endAnalysis() {}
    virtual void handleData(const char* data, uint32_t length) = 0;
    virtual bool isReadyWithStream() = 0;
};

class STREAMANALYZER_EXPORT StreamEventAnalyzerFactory {
public:
    virtual ~StreamEventAnalyzerFactory() {}
    virtual const char* getName() const = 0;
    virtual void registerFields(Strigi::FieldRegister&) = 0;
    virtual StreamEventAnalyzer* newInstance() const = 0;
};


}

#endif
