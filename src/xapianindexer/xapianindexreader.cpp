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
#include "xapianindexreader.h"
#include "xapianindexmanager.h"
#include <xapian.h>
#include <set>
#include <sstream>
#include <assert.h>
using namespace std;
using namespace Strigi;
using namespace Xapian;

XapianIndexReader::XapianIndexReader(XapianIndexManager* m, WritableDatabase* d)
    : manager(m), db(d) {
}
XapianIndexReader::~XapianIndexReader() {
}
int
XapianIndexReader::countHits(const Strigi::Query& query) {
    return -1;
}
vector<IndexedDocument>
XapianIndexReader::query(const Strigi::Query& query) {
    std::vector<IndexedDocument> results;
    return results;
}
map<string, time_t>
XapianIndexReader::getFiles(char depth) {
    map<string, time_t> files;
    return files;
}
int
XapianIndexReader::countDocuments() {
    int count = 0;
    manager->ref();
    manager->deref();
    return count;
}
