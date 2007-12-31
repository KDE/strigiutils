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
#ifndef STRIGI_WAVTHROUGHANALYZER
#define STRIGI_WAVTHROUGHANALYZER

#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"

#include <string>

namespace Strigi {
    class RegisteredField;
}
class WavThroughAnalyzerFactory;

class STRIGI_PLUGIN_API WavThroughAnalyzer
    : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* analysisResult;
    const WavThroughAnalyzerFactory* factory;
public:
    WavThroughAnalyzer(const WavThroughAnalyzerFactory* f) :factory(f) {}
    ~WavThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult* i);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "WavThroughAnalyzer"; }
};

class WavThroughAnalyzerFactory
    : public Strigi::StreamThroughAnalyzerFactory {
friend class WavThroughAnalyzer;
private:
    static const std::string sampleSizeFieldName;
    static const std::string sampleRateFieldName;
    static const std::string channelsFieldName;
    static const std::string lengthFieldName;
    const Strigi::RegisteredField* sampleSizeField;
    const Strigi::RegisteredField* sampleRateField;
    const Strigi::RegisteredField* channelsField;
    const Strigi::RegisteredField* lengthField;

    const char* name() const {
        return "WavThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new WavThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
