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
#ifndef IDV32THROUGHANALYZER_H
#define IDV32THROUGHANALYZER_H

#include "streamthroughanalyzer.h"
#include "fieldtypes.h"

// id3v2 according to http://www.id3.org/id3v2.4.0-structure.txt

class ID3V2ThroughAnalyzerFactory;
class ID3V2ThroughAnalyzer : public Strigi::StreamThroughAnalyzer {
private:
    Strigi::AnalysisResult* indexable;
    const ID3V2ThroughAnalyzerFactory* factory;
public:
    ID3V2ThroughAnalyzer(const ID3V2ThroughAnalyzerFactory* f)
        :indexable(0), factory(f) {}
    void setIndexable(Strigi::AnalysisResult*);
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream();
    const char* name() const { return "ID3V2ThroughAnalyzer"; }
};

class ID3V2ThroughAnalyzerFactory
        : public Strigi::StreamThroughAnalyzerFactory {
friend class ID3V2ThroughAnalyzer;
private:
    static const std::string titleFieldName;
    static const std::string artistFieldName;
    static const std::string albumFieldName;
    static const std::string composerFieldName;
    static const std::string genreFieldName;
    static const std::string trackNumberFieldName;
    static const std::string albumTrackCountFieldName;
    static const std::string discNumberFieldName;
    const Strigi::RegisteredField* titleField;
    const Strigi::RegisteredField* artistField;
    const Strigi::RegisteredField* albumField;
    const Strigi::RegisteredField* composerField;
    const Strigi::RegisteredField* genreField;
    const Strigi::RegisteredField* trackNumberField;
    const Strigi::RegisteredField* albumTrackCountField;
    const Strigi::RegisteredField* discNumberField;
    const char* name() const {
        return "ID3V2ThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new ID3V2ThroughAnalyzer(this);
    }
    void registerFields(Strigi::FieldRegister&);
};


#endif
