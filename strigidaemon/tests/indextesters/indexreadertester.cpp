/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#include "indexreadertester.h"

#include <strigi/analysisresult.h>
#include <strigi/indexwriter.h>
#include <strigi/indexreader.h>
#include <strigi/fieldtypes.h>
#include <strigi/query.h>
#include <strigi/queryparser.h>

#include <sstream>
#include <ostream>

using namespace std;
using namespace strigiunittest;
using namespace Strigi;

void
IndexReaderTest::setUp() {
    IndexTest::setUp();
    m_streamAnalyzer = new Strigi::StreamAnalyzer( m_analyzerConfiguration );
}

void
IndexReaderTest::tearDown() {
    delete m_streamAnalyzer;
    IndexTest::tearDown();
}

void
IndexReaderTest::testChildrenRetrieval() {
    // FIXME
}

void
IndexReaderTest::addAndCount() {
    static const int m = 20;

    m_writer->deleteAllEntries();
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { AnalysisResult idx(s, 0, *m_writer, *m_streamAnalyzer); }
        str.str("");
    }
    m_writer->commit();

    int n = m_reader->countDocuments();

    str.str("");
    CPPUNIT_ASSERT_EQUAL_MESSAGE(str.str(), m, n);
}

void
IndexReaderTest::testNumberQuery() {
    m_writer->deleteAllEntries();
    // add numbers to the database
    int m = 200;
    ostringstream str;
    for (int i=1; i<=m; ++i) {
        str << i;
        string value(str.str());
        string name('/'+value);
        {
            AnalysisResult idx(name, 0, *m_writer, *m_streamAnalyzer);
            idx.addValue(idx.config().fieldRegister().sizeField, value);
        }
        str.str("");
    }
    m_writer->commit();
    QueryParser parser;
    Query q = parser.buildQuery("contentSize>0");
    int count = m_reader->countHits(q);

    str.str("");
    CPPUNIT_ASSERT_EQUAL_MESSAGE(str.str(), m, count);
}
