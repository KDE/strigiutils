/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Vincent Ricard <magic@magicninja.org>
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
#ifndef STRIGI_XPMLINEANALYZER_H
#define STRIGI_XPMLINEANALYZER_H

#include <strigi/streamlineanalyzer.h>
#include <strigi/analyzerplugin.h>

namespace Strigi {
    class RegisteredField;
}
class XpmLineAnalyzerFactory;

class STRIGI_PLUGIN_API XpmLineAnalyzer
    : public Strigi::StreamLineAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const XpmLineAnalyzerFactory* factory;
    int32_t line;
    bool ready;
public:
    XpmLineAnalyzer(const XpmLineAnalyzerFactory* f) :factory(f) {}
    ~XpmLineAnalyzer() {}
    const char* name() const { return "XpmLineAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void endAnalysis(bool complete) {}
    void handleLine(const char* data, uint32_t length);
    bool isReadyWithStream();
};

class XpmLineAnalyzerFactory
    : public Strigi::StreamLineAnalyzerFactory {
friend class XpmLineAnalyzer;
private:
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* numberOfColorsField;

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "XpmLineAnalyzer";
    }
    Strigi::StreamLineAnalyzer* newInstance() const {
        return new XpmLineAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
