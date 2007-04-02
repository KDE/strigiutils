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
#include "strigiconfig.h"
#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "analysisresult.h"
#include "indexmanager.h"
#include "analyzerconfiguration.h"
#include "query.h"
#include "strigi_thread.h"
#include <sstream>
#include <iostream>
using namespace std;
using namespace Strigi;

class IndexManagerTester {
private:
    StrigiMutex lock;
    IndexManager* manager;
    IndexWriter* writer;
    IndexReader* reader;
    StreamAnalyzer si;
public:
    IndexManagerTester(IndexManager* m, AnalyzerConfiguration& ic)
            : manager(m), writer(manager->indexWriter()),
              si(ic) {
        reader = manager->indexReader();
    }
    ~IndexManagerTester() {
    }
    void runUnthreadedTests();
    void runThreadedTests();
    void addAndCount(int m = 20);
    void testNumberQuery();
};
void
IndexManagerTester::runUnthreadedTests() {

    // tests that only need return 0 when not threaded
    addAndCount();
    testNumberQuery();

}
void
IndexManagerTester::runThreadedTests() {
    // tests that only need return 0 when not threaded
    addAndCount();
    testNumberQuery();
}
void
IndexManagerTester::addAndCount(int m) {
    VERIFY(writer);
    if (writer == 0) return;
    writer->deleteAllEntries();
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { AnalysisResult idx(s, 0, *writer, si); }
        str.str("");
    }
    writer->commit();
    int n = reader->countDocuments();
    if (n != m) fprintf(stderr, "%i != %i\n", n, m);
    VERIFY(n == m);
}
void
IndexManagerTester::testNumberQuery() {
    VERIFY(writer);
    if (writer == 0) return;
    writer->deleteAllEntries();
    // add numbers to the database
    int m = 200;
    ostringstream str;
    string size("size");
    for (int i=1; i<=m; ++i) {
        str << i;
        string value(str.str());
        string name('/'+value);
        {
             AnalysisResult idx(name, 0, *writer, si);
             idx.addValue(idx.config().fieldRegister().sizeField, value);
        }
        str.str("");
    }
    writer->commit();
    QueryParser parser;
    Query q = parser.buildQuery("size:>0", -1, 0);
    int count = reader->countHits(q);
    if (count != m) fprintf(stderr, "%i != %i\n", count, m);
    VERIFY(count == m);
}
/* below here the threading plumbing is done */
STRIGI_THREAD_FUNCTION(threadstarter,d) {
//    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
//    tester->runThreadedTests();
    STRIGI_THREAD_EXIT(0);
    return 0;
}
IndexManagerTests::IndexManagerTests(Strigi::IndexManager* m,
        AnalyzerConfiguration& ic)
    :tester (new IndexManagerTester(m, ic)) {
}
IndexManagerTests::~IndexManagerTests() {
    delete tester;
}
void
IndexManagerTests::testAllInThreads(int n) {
    STRIGI_THREAD_TYPE* thread = new STRIGI_THREAD_TYPE[n];
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_CREATE(&thread[i], threadstarter, this);
    }
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_JOIN(thread[i]);
    }

    delete [] thread;
}

void
IndexManagerTests::testAll() {
    tester->runUnthreadedTests();
    tester->runThreadedTests();
}
void
IndexManagerTests::runUnthreadedTests() {
    tester->runUnthreadedTests();
}
void
IndexManagerTests::runThreadedTests() {
    tester->runThreadedTests();
}
