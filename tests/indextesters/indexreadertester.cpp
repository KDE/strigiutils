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

#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "indexmanager.h"
#include "indexwriter.h"
#include "indexreader.h"
#include "fieldtypes.h"
#include "analyzerconfiguration.h"
#include "query.h"
#include "queryparser.h"

#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

#include <ostream>

using namespace std;
using namespace strigiunittest;
using namespace Strigi;

void IndexReaderTester::setUp()
{
    m_manager = createManager();
    m_writer = m_manager->indexWriter();
    m_reader = m_manager->indexReader();

    CPPUNIT_ASSERT_MESSAGE("writer creation failed", m_writer);
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", m_reader);

    m_streamAnalyzer = new Strigi::StreamAnalyzer( m_analyzerConfiguration );
}

void IndexReaderTester::tearDown()
{
    delete m_streamAnalyzer;
    deleteManager( m_manager );
}


void IndexReaderTester::deleteManager( Strigi::IndexManager* m )
{
    delete m;
}

void
IndexReaderTester::testChildrenRetrieval() {
    // FIXME
}

void IndexReaderTester::addAndCount()
{
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

void IndexReaderTester::testNumberQuery()
{
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
    Query q = parser.buildQuery("size>0");
    int count = m_reader->countHits(q);

    str.str("");
    CPPUNIT_ASSERT_EQUAL_MESSAGE(str.str(), m, count);
}
