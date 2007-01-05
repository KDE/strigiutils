/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
#include "indexreadertests.h"
#include "indexreader.h"
using namespace jstreams;

class IndexReaderTester {
private:
    IndexReader* reader;
public:
    IndexReaderTester(IndexReader* r) :reader(r) {}
    int getFiles(char depth) {
        VERIFY(reader);
        if (reader == 0) return 1;
        reader->getFiles(depth);
        return 0;
    }
};

IndexReaderTests::IndexReaderTests(jstreams::IndexReader* w)
    :tester (new IndexReaderTester(w)) {
}
IndexReaderTests::~IndexReaderTests() {
    delete tester;
}

int
IndexReaderTests::testAll() {
    int n = 0;
    n += tester->getFiles(0);
    return n;
}
