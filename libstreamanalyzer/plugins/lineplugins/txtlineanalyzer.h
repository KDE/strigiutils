/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#ifndef STRIGI_TXTLINEANALYZER
#define STRIGI_TXTLINEANALYZER

#include <strigi/streamlineanalyzer.h>
#include <strigi/analyzerplugin.h>

namespace Strigi {
    class RegisteredField;
}
class TxtLineAnalyzerFactory;

class STRIGI_PLUGIN_API TxtLineAnalyzer
    : public Strigi::StreamLineAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const TxtLineAnalyzerFactory* factory;
    int totalWords;
    int totalCharacters;
    int totalLines;
    uint32_t maxLineLength;
    bool dos;
    bool ready;
public:
    TxtLineAnalyzer(const TxtLineAnalyzerFactory* f) :factory(f) {}
    ~TxtLineAnalyzer() {}
    const char* name() const { return "TxtLineAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void handleLine(const char* data, uint32_t length);
    void endAnalysis(bool complete);
    bool isReadyWithStream();
};

class TxtLineAnalyzerFactory
    : public Strigi::StreamLineAnalyzerFactory {
friend class TxtLineAnalyzer;
private:
    const Strigi::RegisteredField* totalLinesField;
    const Strigi::RegisteredField* totalCharactersField;
    const Strigi::RegisteredField* totalWordsField;
    const Strigi::RegisteredField* maxLineLengthField;
    const Strigi::RegisteredField* formatField;

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "TxtLineAnalyzer";
    }
    Strigi::StreamLineAnalyzer* newInstance() const {
        return new TxtLineAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif

