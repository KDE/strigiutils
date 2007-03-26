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

#include "jstreamsconfig.h"
#include "analysisresult.h"
#include "indexwriter.h"
#include "analyzerconfiguration.h"
#include "streamanalyzer.h"
#include "streambase.h"
#include <string>
using namespace Strigi;
using namespace jstreams;

AnalysisResult::AnalysisResult(const std::string& p, const char* name,
        time_t mt, AnalysisResult& parent)
            :m_writerData(0), m_mtime(mt), m_name(name), m_path(p),
             m_writer(parent.m_writer), m_depth(parent.depth()+1),
             m_indexer(parent.m_indexer),
             m_analyzerconfig(parent.m_analyzerconfig), m_parent(&parent),
             m_endanalyzer(0) {
    m_writer.startAnalysis(this);
}
AnalysisResult::AnalysisResult(const std::string& p, time_t mt, IndexWriter& w,
        StreamAnalyzer& indexer)
            :m_writerData(0), m_mtime(mt), m_path(p), m_writer(w), m_depth(0),
             m_indexer(indexer), m_analyzerconfig(indexer.getConfiguration()),
             m_parent(0), m_endanalyzer(0) {
    size_t pos = m_path.rfind('/');
    if (pos == std::string::npos) {
        m_name = m_path;
    } else {
        m_name = m_path.substr(pos+1);
    }
    m_writer.startAnalysis(this);
}
AnalysisResult::~AnalysisResult() {
    m_writer.finishAnalysis(this);
}

const std::string& AnalysisResult::fileName() const { return m_name; }
const std::string& AnalysisResult::path() const { return m_path; }
time_t AnalysisResult::mTime() const { return m_mtime; }
char AnalysisResult::depth() const { return m_depth; }
int64_t AnalysisResult::id() const { return m_id; }
void AnalysisResult::setId(int64_t i) { m_id = i; }
void AnalysisResult::setEncoding(const char* enc) { m_encoding = enc; }
const std::string& AnalysisResult::encoding() const { return m_encoding; }
void* AnalysisResult::writerData() const { return m_writerData; }
void AnalysisResult::setWriterData(void* wd) { m_writerData = wd; }
void AnalysisResult::setMimeType(const std::string& mt) { m_mimetype = mt; }
const std::string& AnalysisResult::mimeType() const { return m_mimetype; }

char
AnalysisResult::index(jstreams::StreamBase<char>* file) {
    return m_indexer.analyze(*this, file);
}
char
AnalysisResult::indexChild(const std::string& name, time_t mt,
        jstreams::StreamBase<char>* file) {
    std::string path(this->m_path);
    path.append("/");
    path.append(name);
    const char* n = path.c_str() + path.rfind('/') + 1;
    // check if we should index this file by applying the filename filters
    if (m_analyzerconfig.indexFile(path.c_str(), n)) {
        AnalysisResult i(path, n, mt, *this);
        return m_indexer.analyze(i, file);
    }
    return 0;
}
void
AnalysisResult::addText(const char* text, int32_t length) {
    m_writer.addText(this, text, length);
}
AnalyzerConfiguration&
AnalysisResult::config() const {
    return m_analyzerconfig;
}
AnalysisResult*
AnalysisResult::parent() {
    return m_parent;
}
const StreamEndAnalyzer*
AnalysisResult::endAnalyzer() const {
    return m_endanalyzer;
}
void
AnalysisResult::setEndAnalyzer(const StreamEndAnalyzer* ea) {
    m_endanalyzer = ea;
}

