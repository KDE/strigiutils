/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef OLEENDANALYZER
#define OLEENDANALYZER

#include "streamendanalyzer.h"
#include "streambase.h"
#include <map>

class OleEndAnalyzerFactory;
class OleEndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const OleEndAnalyzerFactory* const factory;
    Strigi::AnalysisResult* result;

    void handleProperty(const Strigi::RegisteredField* field, const char* data);
    void handlePropertyStream(const char* key, const char* data, const char* end);
public:
    OleEndAnalyzer(const OleEndAnalyzerFactory* const f) :factory(f) {}
    bool checkHeader(const char* header, int32_t headersize) const;
    bool tryPropertyStream(Strigi::AnalysisResult& idx, Strigi::InputStream* s);
    char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "OleEndAnalyzer"; }
};

class OleEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
private:
    std::map<std::string,
        std::map<int,const Strigi::RegisteredField*> > fieldsMaps;
public:
    const char* name() const {
        return "OleEndAnalyzer";
    }
    Strigi::StreamEndAnalyzer* newInstance() const {
        return new OleEndAnalyzer(this);
    }
    bool analyzesSubStreams() const { return true; }
    void registerFields(Strigi::FieldRegister&);
    const std::map<int, const Strigi::RegisteredField*>* getFieldMap(
        const std::string& key) const;
};


#endif
