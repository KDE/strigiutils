/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#ifndef FLACTHROUGHANALYZER_H
#define FLACTHROUGHANALYZER_H

#include "streamthroughanalyzer.h"
#include "fieldtypes.h"
#include <map>

// flac according to http://flac.sourceforge.net/format.html

class FlacThroughAnalyzerFactory;
class FlacThroughAnalyzer : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* indexable;
    const FlacThroughAnalyzerFactory* factory;
public:
    FlacThroughAnalyzer(const FlacThroughAnalyzerFactory* f) :factory(f) {}
    ~FlacThroughAnalyzer() {}
    void setIndexable(Strigi::AnalysisResult*);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "FlacThroughAnalyzer"; }
};

class FlacThroughAnalyzerFactory
        : public Strigi::StreamThroughAnalyzerFactory {
friend class FlacThroughAnalyzer;
private:
    std::map<std::string, const Strigi::RegisteredField*> fields;
    const Strigi::RegisteredField* artistField;
    const Strigi::RegisteredField* albumField;
    const Strigi::RegisteredField* composerField;
    const Strigi::RegisteredField* performerField;
    const char* name() const {
        return "FlacThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new FlacThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
