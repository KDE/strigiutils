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
#ifndef STRIGI_TGATHROUGHANALYZER
#define STRIGI_TGATHROUGHANALYZER

#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

namespace Strigi {
    class RegisteredField;
}
class TgaThroughAnalyzerFactory;

class STRIGI_PLUGIN_API TgaThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const TgaThroughAnalyzerFactory* factory;
public:
    TgaThroughAnalyzer(const TgaThroughAnalyzerFactory* f) :factory(f) {}
    ~TgaThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
};

class TgaThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class TgaThroughAnalyzer;
private:
    const Strigi::RegisteredField* colorDepthField;
    const Strigi::RegisteredField* colorModeField;
    const Strigi::RegisteredField* compressionField;
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "TgaThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new TgaThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
