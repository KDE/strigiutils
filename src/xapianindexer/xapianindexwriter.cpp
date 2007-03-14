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
#include "xapianindexwriter.h"
#include "xapianindexmanager.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace jstreams;
using namespace Xapian;

XapianIndexWriter::XapianIndexWriter(XapianIndexManager *m,
    Xapian::WritableDatabase* d) : manager(m), db(d) {
}
XapianIndexWriter::~XapianIndexWriter() {
}
void
XapianIndexWriter::addText(const AnalysisResult* idx, const char* text,
        int32_t length) {
}
void
XapianIndexWriter::setField(const AnalysisResult* idx, const string& name,
        const string& value) {
}

void
XapianIndexWriter::startIndexable(AnalysisResult* idx) {
    Document *doc = new Document();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
XapianIndexWriter::finishIndexable(const AnalysisResult* idx) {
    static const string path("path"), mtime("mtime"), depth("depth");
    Document* doc = static_cast<Document*>(idx->getWriterData());
    manager->ref();
    db->add_document(*doc);
    manager->deref();
    delete doc;
}
void
XapianIndexWriter::commit() {
}
/**
 * Delete all files that start with the specified path.
 **/
void
XapianIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
}
void
XapianIndexWriter::deleteEntry(const string& path) {
}
