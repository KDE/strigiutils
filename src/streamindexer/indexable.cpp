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
#include "indexable.h"
#include "indexwriter.h"
#include "indexerconfiguration.h"
#include "streamindexer.h"
#include "streambase.h"
#include <string>
using namespace jstreams;

Indexable::Indexable(const std::string& path, const char* name, time_t mt,
        const Indexable& parent)
            :mtime(mt), name(name), path(parent.path+'/'+name),
             writer(parent.writer), depth(parent.getDepth()+1),
             indexer(parent.indexer), indexableconfig(parent.indexableconfig) {
    writer.startIndexable(this);
}
Indexable::Indexable(const std::string& p, time_t mt, IndexWriter& w,
        StreamIndexer& indexer)
            :mtime(mt), path(p), writer(w), depth(0), indexer(indexer),
             indexableconfig(indexer.getConfiguration()) {
    size_t pos = path.rfind('/');
    if (pos == std::string::npos) {
        name = path;
    } else {
        name = path.substr(pos+1);
    }
    writer.startIndexable(this);
}
Indexable::~Indexable() {
    writer.finishIndexable(this);
}

const std::string& Indexable::getFileName() const { return name; }
const std::string& Indexable::getPath() const { return path; }
time_t Indexable::getMTime() const { return mtime; }
char Indexable::getDepth() const { return depth; }
int64_t Indexable::getId() const { return id; }
void Indexable::setId(int64_t i) { id = i; }
void Indexable::setEncoding(const char* enc) { encoding = enc; }
const std::string& Indexable::getEncoding() const { return encoding; }
void* Indexable::getWriterData() const { return writerData; }
void Indexable::setWriterData(void* wd) { writerData = wd; }
void Indexable::setMimeType(const std::string& mt) { mimetype = mt; }
const std::string& Indexable::getMimeType() const { return mimetype; }

char
Indexable::index(StreamBase<char>& file) {
    return indexer.analyze(*this, &file);
}
char
Indexable::indexChild(const std::string& name, time_t mt,
        StreamBase<char>& file) {
    std::string path(this->path);
    path.append("/");
    path.append(name);
    const char* n = path.c_str() + path.rfind('/') + 1;
    // check if we should index this file by applying the filename filters
    if (indexableconfig.indexFile(path.c_str(), n)) {
        Indexable i(path, n, mt, *this);
        return indexer.analyze(i, &file);
    }
    return 0;
}
void
Indexable::addText(const char* text, int32_t length) {
    writer.addText(this, text, length);
}
IndexerConfiguration&
Indexable::config() const {
    return indexableconfig;
}
