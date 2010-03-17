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
#include <CLucene.h>
#include <CLucene/search/QueryFilter.h>
#include "../cluceneindexmanager.h"
#include "../tcharutils.h"
#include <sstream>
#include <iostream>
#include <cassert>

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::Term;
using lucene::index::TermEnum;
using lucene::search::TermQuery;
using lucene::search::WildcardQuery;
using lucene::search::BooleanQuery;
using lucene::search::RangeQuery;
using lucene::search::QueryFilter;
using lucene::search::HitCollector;
using lucene::util::BitSet;
using lucene::document::DocumentFieldEnumeration;
using lucene::index::IndexReader;

void
docdump(Document* doc) {
    DocumentFieldEnumeration* e = doc->fields();
    Field* f = e->nextElement();
    while (f) {
        f = e->nextElement();
    }
    delete e;
}
std::string
t2a(const TCHAR* t) {
    return wchartoutf8(t);
}
void
indexdump(const char* dir) {
    IndexReader* indexreader = IndexReader::open(dir);
    int32_t max = indexreader->maxDoc();
    for (int i=0; i<max; ++i) {
        Document* doc = indexreader->document(i);
        if (doc) {
            docdump(doc);
        }
    }
    TermEnum* terms = indexreader->terms();
    while (terms->next()) {
        Term* t = terms->term();
        printf("%s: %s\n", t2a(t->field()).c_str(), t2a(t->text()).c_str());
        _CLDECDELETE(t);
    }
}
int
main(int argc, char** argv) {
    try {
        for (int i=1; i<argc; ++i) {
            indexdump(argv[i]);
        }
    } catch (...) {
		fprintf(stderr,"error while dumping index\n");
    }
    return 0;
}
