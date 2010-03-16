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
#ifndef STRIGI_DIGESTEVENTANALYZER_H
#define STRIGI_DIGESTEVENTANALYZER_H

#include <strigi/streameventanalyzer.h>
#include "../sha1.h"

namespace Strigi {
    class RegisteredField;
    class FieldRegister;
}

class DigestEventAnalyzerFactory;
class DigestEventAnalyzer : public Strigi::StreamEventAnalyzer {
private:
    SHA1 sha1;
    Strigi::AnalysisResult* analysisresult;
    const DigestEventAnalyzerFactory* const factory;
public:
    DigestEventAnalyzer(const DigestEventAnalyzerFactory*);
    ~DigestEventAnalyzer();
    const char* name() const { return "DigestEventAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void endAnalysis(bool complete);
    void handleData(const char* data, uint32_t length);
    bool isReadyWithStream();
};

class DigestEventAnalyzerFactory
        : public Strigi::StreamEventAnalyzerFactory {
public:
    const Strigi::RegisteredField* shafield;
private:
    const char* name() const {
        return "DigestEventAnalyzer";
    }
    void registerFields(Strigi::FieldRegister&);
    Strigi::StreamEventAnalyzer* newInstance() const {
        return new DigestEventAnalyzer(this);
    }
};

#endif
