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
#ifndef DUMMYINDEXWRITER_H
#define DUMMYINDEXWRITER_H

#include <strigi/analysisresult.h>
#include <strigi/indexwriter.h>
#include <strigi/indexmanager.h>
#include <strigi/fieldtypes.h>
#include <cstring>

class DummyIndexWriter : public Strigi::IndexWriter {
private:
    int verbosity;
protected:
    void startAnalysis(const Strigi::AnalysisResult* ar) {
        if (verbosity >= 1) {
            printf("%s\n", ar->path().c_str());
        }
        if (verbosity == -1) { // sha1 mode
            std::string* s = new std::string();
            ar->setWriterData(s);
        }
    }
    void finishAnalysis(const Strigi::AnalysisResult* ar) {
        if (verbosity == -1) { // sha1 mode
            const std::string* s = static_cast<const std::string*>(
                ar->writerData());
            printf("%s\t%s\n", ar->path().c_str(), s->c_str());
            delete s;
        }
    }
    void addText(const Strigi::AnalysisResult* ar, const char* text,
        int32_t length) {
        if (verbosity > 2) {
            printf("%s: addText '%.*s'\n", ar->path().c_str(), length,
                text);
        }
    }
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* field, const std::string& value) {
        if (verbosity > 1) {
            printf("%s: setField '%s': '%s'\n", ar->path().c_str(),
                field->key().c_str(), value.c_str());
        } else if (verbosity == -1
                && std::strcmp(field->key().c_str(), "sha1") == 0) {
            std::string* s = static_cast<std::string*>(ar->writerData());
            *s = value;
        }
    }
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, const unsigned char* data,
        uint32_t size) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, uint32_t value) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, int32_t value) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, double value) {}
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* field, const std::string& name,
        const std::string& value) {}
public:
    DummyIndexWriter(int v = 0) {
        verbosity = v;
    }
    ~DummyIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};

class DummyIndexManager : public Strigi::IndexManager {
private:
    DummyIndexWriter writer;
public:
    DummyIndexManager(int level) :writer(level) {}
    Strigi::IndexWriter* indexWriter() {
        return &writer;
    }
    Strigi::IndexReader* indexReader() {
        return 0;
    }
};

#endif
