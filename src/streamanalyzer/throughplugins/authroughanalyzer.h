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
#ifndef AUTHROUGHANALYZER
#define AUTHROUGHANALYZER

#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"
#include "string.h"

namespace Strigi {
    class RegisteredField;
}
class AuThroughAnalyzerFactory;

class STRIGI_PLUGIN_API AuThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const AuThroughAnalyzerFactory* factory;
public:
    AuThroughAnalyzer(const AuThroughAnalyzerFactory* f) :factory(f) {}
    ~AuThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
};

class AuThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class AuThroughAnalyzer;
private:
    static const std::string lengthFieldName;
    static const std::string sampleRateFieldName;
    static const std::string channelsFieldName;
    static const std::string encodingFieldName;
    const Strigi::RegisteredField* lengthField;
    const Strigi::RegisteredField* sampleRateField;
    const Strigi::RegisteredField* channelsField;
    const Strigi::RegisteredField* encodingField;
    const char* name() const {
        return "AuThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new AuThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
