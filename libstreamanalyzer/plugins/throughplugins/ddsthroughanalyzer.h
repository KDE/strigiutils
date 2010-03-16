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
#ifndef STRIGI_DDSTHROUGHANALYZER
#define STRIGI_DDSTHROUGHANALYZER

#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

#include <string>

namespace Strigi {
    class RegisteredField;
}
class DdsThroughAnalyzerFactory;

class STRIGI_PLUGIN_API DdsThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const DdsThroughAnalyzerFactory* factory;
public:
    DdsThroughAnalyzer(const DdsThroughAnalyzerFactory* f) :factory(f) {}
    ~DdsThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "DdsThroughAnalyzer"; }
};

class DdsThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class DdsThroughAnalyzer;
private:
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* volumeDepthField;
    const Strigi::RegisteredField* bitDepthField;
    const Strigi::RegisteredField* mipmapCountField;
    const Strigi::RegisteredField* typeField;
    const Strigi::RegisteredField* colorModeField;
    const Strigi::RegisteredField* compressionField;

    const char* name() const {
        return "DdsThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new DdsThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
