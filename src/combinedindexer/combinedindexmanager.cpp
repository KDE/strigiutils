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
#include "combinedindexmanager.h"
#ifdef HAVE_CLUCENE
#include "cluceneindexmanager.h"
#endif
#ifdef HAVE_XAPIAN
#include "xapianindexmanager.h"
#endif
#ifdef HAVE_ESTRAIER
#include "estraierindexmanager.h"
#endif
#ifdef HAVE_SQLITE
#include "sqliteindexmanager.h"
#endif
#include <string>
#include <map>
using namespace std;
using namespace jstreams;

std::map<string, IndexManager*(*)(const char*)>
CombinedIndexManager::getFactories() {
    map<string, IndexManager*(*)(const char*)> factories;
#ifdef HAVE_ESTRAIER
    factories["estraier"] = createEstraierIndexManager;
#endif
#ifdef HAVE_CLUCENE
    factories["clucene"] = createCLuceneIndexManager;
#endif
#ifdef HAVE_XAPIAN
    factories["xapian"] = createXapianIndexManager;
#endif
#ifdef HAVE_SQLITE
    factories["sqlite"] = createSqliteIndexManager;
#endif
    return factories;
}

vector<string>
CombinedIndexManager::getBackEnds() {
    vector<string> v;
    std::map<string, IndexManager*(*)(const char*)> f = getFactories();
    map<string, IndexManager*(*)(const char*)>::const_iterator i;
    for (i=f.begin(); i!=f.end(); ++i) {
        v.push_back(i->first);
    }
    return v;
}
CombinedIndexManager::CombinedIndexManager(const string& type,
        const string& indexdir) {
    // determine the right index manager
    map<string, IndexManager*(*)(const char*)> factories = getFactories();
    map<string, IndexManager*(*)(const char*)>::const_iterator f
        = factories.find(type);
    if (f == factories.end()) {
        f = factories.begin();
    }
    writermanager = f->second(indexdir.c_str());
}
CombinedIndexManager::~CombinedIndexManager() {
    delete writermanager;
}
jstreams::IndexReader*
CombinedIndexManager::getIndexReader() {
    return writermanager->getIndexReader();
}
jstreams::IndexWriter*
CombinedIndexManager::getIndexWriter() {
    return writermanager->getIndexWriter();
}
