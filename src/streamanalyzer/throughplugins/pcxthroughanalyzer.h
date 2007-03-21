/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#ifndef PCXTHROUGHANALYZER_H
#define PCXTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
#include "analyzerplugin.h"
#include "cnstr.h"

class PcxThroughAnalyzerFactory;
class PcxThroughAnalyzer : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* indexable;
    const PcxThroughAnalyzerFactory* factory;
public:
    PcxThroughAnalyzer(const PcxThroughAnalyzerFactory* f) :factory(f) {}
    ~PcxThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult*);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
    bool isReadyWithStream();
};

class PcxThroughAnalyzerFactory
        : public Strigi::StreamThroughAnalyzerFactory {
friend class PcxThroughAnalyzer;
private:
    static const cnstr widthFieldName;
    static const cnstr heightFieldName;
    static const cnstr colorDepthFieldName;
    static const cnstr compressionFieldName;
    static const cnstr hResolutionFieldName;
    static const cnstr vResolutionFieldName;
    
    const Strigi::RegisteredField* widthField;
    const Strigi::RegisteredField* heightField;
    const Strigi::RegisteredField* colorDepthField;
    const Strigi::RegisteredField* compressionField;
    const Strigi::RegisteredField* hResolutionField;
    const Strigi::RegisteredField* vResolutionField;
    const char* getName() const {
        return "PcxThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new PcxThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};

#endif
