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
#ifndef STRIGI_RGBTHROUGHANALYZER
#define STRIGI_RGBTHROUGHANALYZER

#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

#include <string>

namespace Strigi {
    class RegisteredField;
}
class RgbThroughAnalyzerFactory;

class STRIGI_PLUGIN_API RgbThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const RgbThroughAnalyzerFactory* factory;
public:
    RgbThroughAnalyzer(const RgbThroughAnalyzerFactory* f) :factory(f) {}
    ~RgbThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "RgbThroughAnalyzer"; }
};

class RgbThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class RgbThroughAnalyzer;
private:
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* bitDepthField;
    const Strigi::RegisteredField* imageNameField;
    const Strigi::RegisteredField* sharedRowsField;
    const Strigi::RegisteredField* colorModeField;
    const Strigi::RegisteredField* compressionField;

    const char* name() const {
        return "RgbThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new RgbThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
