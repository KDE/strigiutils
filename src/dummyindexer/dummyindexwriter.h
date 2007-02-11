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

#include "indexable.h"
#include "indexwriter.h"
#include "fieldtypes.h"

class DummyIndexWriter : public jstreams::IndexWriter {
private:
    int verbosity;
protected:
    void startIndexable(jstreams::Indexable* idx) {
        if (verbosity >= 1) {
            printf("%s\n", idx->getPath().c_str());
        }
        if (verbosity == -1) { // sha1 mode
            std::string* s = new std::string();
            idx->setWriterData(s);
        }
    }
    void finishIndexable(const jstreams::Indexable* idx) {
        if (verbosity == -1) { // sha1 mode
            const std::string* s = static_cast<const std::string*>(
                idx->getWriterData());
            printf("%s\t%s\n", idx->getPath().c_str(), s->c_str());
            delete s;
        }
    }
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length) {
        if (verbosity > 2) {
            printf("%s: addText '%.*s'\n", idx->getPath().c_str(), length,
                text);
        }
    }
    void addField(const jstreams::Indexable* idx,
        const jstreams::RegisteredField* field, const std::string& value) {
        if (verbosity > 1) {
            printf("%s: setField '%s': '%s'\n", idx->getPath().c_str(),
                (const char*)field->getKey(), value.c_str());
        } else if (verbosity == -1
                && strcmp(field->getKey(), "sha1") == 0) {
            std::string* s = static_cast<std::string*>(idx->getWriterData());
            *s = value;
        }
    }
    void addField(const jstreams::Indexable* idx,
        const jstreams::RegisteredField* fieldname, const unsigned char* data,
        int32_t size) {}
public:
    DummyIndexWriter(int v = 0) {
        verbosity = v;
    }
    ~DummyIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};

#endif
