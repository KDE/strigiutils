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
#ifndef DIGESTEVENTANALYZER_H
#define DIGESTEVENTANALYZER_H

#include "streameventanalyzer.h"

namespace Strigi {
    class RegisteredField;
    class FieldRegister;
}

class DigestEventAnalyzer : public Strigi::StreamEventAnalyzer {
private:
    Strigi::AnalysisResult* analysisresult;
    const Strigi::RegisteredField* shafield;
public:
    DigestEventAnalyzer(Strigi::FieldRegister& reg);
    ~DigestEventAnalyzer();
    const char* name() const { return "DigestEventAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void endAnalysis();
    void handleData(const char* data, uint32_t length);
    bool isReadyWithStream();
};

class DigestEventAnalyzerFactory
        : public Strigi::StreamEventAnalyzerFactory {
private:
    const char* name() const {
        return "DigestEventAnalyzer";
    }
    Strigi::StreamEventAnalyzer* newInstance(Strigi::FieldRegister& reg)
            const {
        return new DigestEventAnalyzer(reg);
    }
};

#endif
