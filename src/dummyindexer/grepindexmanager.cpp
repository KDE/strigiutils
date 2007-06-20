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
#include "grepindexmanager.h"
#include "grepindexreader.h"
#include "indexwriter.h"
using namespace Strigi;

/**
 * Dummy class that does not write anything.
 **/
class DummyGrepIndexWriter : public Strigi::IndexWriter {
public:
    DummyGrepIndexWriter() {}
    void startAnalysis(const AnalysisResult*) {}
    void addText(const AnalysisResult* result, const char* text,
        int32_t length) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const unsigned char* data, uint32_t size) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        int32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        uint32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        double value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& name, const std::string& value) {}
    void finishAnalysis(const AnalysisResult* result) {}
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};
GrepIndexManager::GrepIndexManager(const char* path)
        :reader(0),
         writer(new DummyGrepIndexWriter()){
}
GrepIndexManager::~GrepIndexManager() {
    delete writer;
}
Strigi::IndexReader*
GrepIndexManager::indexReader() {
    return reader;
}
Strigi::IndexWriter*
GrepIndexManager::indexWriter() {
    return writer;
}
Strigi::IndexManager*
createGrepIndexManager(const char* path) {
    return new GrepIndexManager(path);
}
