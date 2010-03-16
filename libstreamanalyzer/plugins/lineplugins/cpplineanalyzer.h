/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 George <grundleborg@gmail.com>
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
#ifndef STRIGI_CPPLINEANALYZER
#define STRIGI_CPPLINEANALYZER

#include <strigi/streamlineanalyzer.h>
#include <strigi/analyzerplugin.h>

namespace Strigi {
    class RegisteredField;
}
class CppLineAnalyzerFactory;

class STRIGI_PLUGIN_API CppLineAnalyzer
    : public Strigi::StreamLineAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const CppLineAnalyzerFactory* factory;
    int codeLines;
    int commentLines;
    int includes;
    bool inComment;
    bool ready;
    bool isCpp;
public:
    CppLineAnalyzer(const CppLineAnalyzerFactory* f) :factory(f) {}
    ~CppLineAnalyzer() {}
    const char* name() const { return "CppLineAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void handleLine(const char* data, uint32_t length);
    void endAnalysis(bool complete);
    bool isReadyWithStream();
};

class CppLineAnalyzerFactory
    : public Strigi::StreamLineAnalyzerFactory {
friend class CppLineAnalyzer;
private:
    const Strigi::RegisteredField* includeField;
    const Strigi::RegisteredField* classField;
    const Strigi::RegisteredField* codeLinesField;
    const Strigi::RegisteredField* commentLinesField;
    const Strigi::RegisteredField* totalLinesField;
    const Strigi::RegisteredField* includesField;
    const Strigi::RegisteredField* programmingLanguageField;

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "CppLineAnalyzer";
    }
    Strigi::StreamLineAnalyzer* newInstance() const {
        return new CppLineAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
