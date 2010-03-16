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
#ifndef STRIGI_ICOTHROUGHANALYZER
#define STRIGI_ICOTHROUGHANALYZER

#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

#include <string>

namespace Strigi {
    class RegisteredField;
}
class IcoThroughAnalyzerFactory;

class STRIGI_PLUGIN_API IcoThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const IcoThroughAnalyzerFactory* factory;
public:
    IcoThroughAnalyzer(const IcoThroughAnalyzerFactory* f) :factory(f) {}
    ~IcoThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "IcoThroughAnalyzer"; }
};

class IcoThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class IcoThroughAnalyzer;
private:
    const Strigi::RegisteredField* numberField;
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* bitsPerPixelField;
    const Strigi::RegisteredField* colorCountField;

    const char* name() const {
        return "IcoThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new IcoThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
