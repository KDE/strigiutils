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

#include <strigi/streamendanalyzer.h>
#include <strigi/streambase.h>
#include <map>
#include <set>
#include <iconv.h>

class WordText {
private:
    std::map<std::string, std::set<std::string> > linkmap;
    iconv_t const windows1252;
    iconv_t const utf16;
    char* out;
    size_t len;
    size_t capacity;

    void addText(const char* d, size_t len, iconv_t conv);
public:
    WordText();
    ~WordText();
    void reset() { len = 0; linkmap.clear(); }
    void addText(const char* d, size_t len);
    void cleanText();
    const char* text() const { return out; }
    size_t length() const { return len; }
    const std::map<std::string, std::set<std::string> >& links() const {
        return linkmap;
    }
};

class OleEndAnalyzerFactory;
class OleEndAnalyzer : public Strigi::StreamEndAnalyzer {
private:
    const OleEndAnalyzerFactory* const factory;
    Strigi::AnalysisResult* result;
    WordText wordtext;

    void handlePropertyStream(const char* key, const char* data,
        const char* end);
    void handleProperty(Strigi::AnalysisResult* result,
        const Strigi::RegisteredField* field,
        const char* data, const char* end);
    std::string getStreamString(Strigi::InputStream*) const;
    bool tryFIB(Strigi::AnalysisResult& ar, Strigi::InputStream* in);
public:
    OleEndAnalyzer(const OleEndAnalyzerFactory* const f) :factory(f) {}
    bool checkHeader(const char* header, int32_t headersize) const;
    bool tryPropertyStream(Strigi::AnalysisResult& idx, Strigi::InputStream* s);
    signed char analyze(Strigi::AnalysisResult& idx, Strigi::InputStream* in);
    const char* name() const { return "OleEndAnalyzer"; }
};

class OleEndAnalyzerFactory : public Strigi::StreamEndAnalyzerFactory {
friend class OleEndAnalyzer;
private:
    std::map<std::string,
        std::map<int,const Strigi::RegisteredField*> > fieldsMaps;
    const Strigi::RegisteredField* typeField;
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
