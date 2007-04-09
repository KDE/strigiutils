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

#include "strigiconfig.h"
#include "analysisresult.h"
#include "indexwriter.h"
#include "analyzerconfiguration.h"
#include "streamanalyzer.h"
#include "streambase.h"
#include "textutils.h"
#include <string>
using namespace Strigi;
using namespace std;

class AnalysisResult::Private {
public:
    int64_t m_id;
    mutable void* m_writerData;
    const time_t m_mtime;
    std::string m_name;
    const std::string m_path;
    std::string m_encoding;
    std::string m_mimetype;
    IndexWriter& m_writer;
    const int m_depth;
    StreamAnalyzer& m_indexer;
    AnalyzerConfiguration& m_analyzerconfig;
    AnalysisResult* const m_parent;
    const StreamEndAnalyzer* m_endanalyzer;

    Private(const std::string& p, const char* name, time_t mt,
        AnalysisResult& parent);
    Private(const std::string& p, time_t mt, IndexWriter& w,
        StreamAnalyzer& indexer);
};

AnalysisResult::Private::Private(const std::string& p, const char* name,
        time_t mt, AnalysisResult& parent)
            :m_writerData(0), m_mtime(mt), m_name(name), m_path(p),
             m_writer(parent.p->m_writer), m_depth(parent.depth()+1),
             m_indexer(parent.p->m_indexer),
             m_analyzerconfig(parent.p->m_analyzerconfig), m_parent(&parent),
             m_endanalyzer(0) {
}
AnalysisResult::AnalysisResult(const std::string& path, const char* name,
        time_t mt, AnalysisResult& parent)
        :p(new Private(path, name, mt, parent)) {
    p->m_writer.startAnalysis(this);
}
AnalysisResult::Private::Private(const std::string& p, time_t mt,
        IndexWriter& w, StreamAnalyzer& indexer)
            :m_writerData(0), m_mtime(mt), m_path(p), m_writer(w), m_depth(0),
             m_indexer(indexer), m_analyzerconfig(indexer.configuration()),
             m_parent(0), m_endanalyzer(0) {
    size_t pos = m_path.rfind('/');
    if (pos == std::string::npos) {
        m_name = m_path;
    } else {
        m_name = m_path.substr(pos+1);
    }
}
AnalysisResult::AnalysisResult(const std::string& path, time_t mt,
        IndexWriter& w, StreamAnalyzer& indexer)
            :p(new Private(path, mt, w, indexer)) {
    p->m_writer.startAnalysis(this);
}
AnalysisResult::~AnalysisResult() {
    p->m_writer.finishAnalysis(this);
    delete p;
}
const std::string& AnalysisResult::fileName() const { return p->m_name; }
const std::string& AnalysisResult::path() const { return p->m_path; }
time_t AnalysisResult::mTime() const { return p->m_mtime; }
char AnalysisResult::depth() const { return p->m_depth; }
int64_t AnalysisResult::id() const { return p->m_id; }
void AnalysisResult::setId(int64_t i) { p->m_id = i; }
void AnalysisResult::setEncoding(const char* enc) { p->m_encoding = enc; }
const std::string& AnalysisResult::encoding() const { return p->m_encoding; }
void* AnalysisResult::writerData() const { return p->m_writerData; }
void AnalysisResult::setWriterData(void* wd) const { p->m_writerData = wd; }
void AnalysisResult::setMimeType(const std::string& mt) { p->m_mimetype = mt; }
const std::string& AnalysisResult::mimeType() const { return p->m_mimetype; }
char
AnalysisResult::index(InputStream* file) {
    return p->m_indexer.analyze(*this, file);
}
char
AnalysisResult::indexChild(const std::string& name, time_t mt,
        InputStream* file) {
    std::string path(p->m_path);
    path.append("/");
    path.append(name);
    const char* n = path.c_str() + path.rfind('/') + 1;
    // check if we should index this file by applying the filename filters
    if (p->m_analyzerconfig.indexFile(path.c_str(), n)) {
        AnalysisResult i(path, n, mt, *this);
        return p->m_indexer.analyze(i, file);
    }
    return 0;
}
void
AnalysisResult::addText(const char* text, int32_t length) {
    p->m_writer.addText(this, text, length);
}
AnalyzerConfiguration&
AnalysisResult::config() const {
    return p->m_analyzerconfig;
}
AnalysisResult*
AnalysisResult::parent() {
    return p->m_parent;
}
const StreamEndAnalyzer*
AnalysisResult::endAnalyzer() const {
    return p->m_endanalyzer;
}
void
AnalysisResult::setEndAnalyzer(const StreamEndAnalyzer* ea) {
    p->m_endanalyzer = ea;
}
string
AnalysisResult::extension() const {
    string::size_type p1 = p->m_name.rfind('.');
    string::size_type p2 = p->m_name.rfind('/');
    if (p1 != string::npos && (p2 == string::npos || p1 > p2)) {
        return p->m_name.substr(p1+1);
    }
    return "";
}
void
AnalysisResult::addValue(const RegisteredField* field, const std::string& value){
    // make sure only utf8 is stored
    if (!checkUtf8(value)) {
        fprintf(stderr, "'%s' is not a UTF8 string\n", value.c_str());
        return;
    }
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field,
        const char* data, uint32_t length) {
    p->m_writer.addValue(this, field, (const unsigned char*)data, length);
}
void
AnalysisResult::addValue(const RegisteredField* field, int32_t value) {
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, uint32_t value) {
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, double value) {
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(RegisteredField*field, const std::string&name,
        const std::string&value) {
    p->m_writer.addValue(this, field, name, value);
}
