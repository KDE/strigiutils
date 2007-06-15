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
#include "indexmanagertester.h"
 
#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "indexmanager.h"
#include "indexwriter.h"
#include "indexreader.h"
#include "query.h"
#include "queryparser.h"

#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;
using namespace strigiunittest;

void IndexManagerTester::setUp()
{
    m = 20;
    manager = NULL;
    writer  = NULL;
    reader  = NULL;
    si      = NULL;
    ic      = NULL;
}

void IndexManagerTester::tearDown()
{
    delete manager;
    delete si;
    delete ic;
    writer = NULL;
    reader = NULL;
}

void IndexManagerTester::testVariables()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("writer  == NULL", writer);
    CPPUNIT_ASSERT_MESSAGE ("reader  == NULL", reader);
    CPPUNIT_ASSERT_MESSAGE ("si == NULL", si);
    CPPUNIT_ASSERT_MESSAGE ("ic == NULL", ic);
}

// void IndexManagerTester::runUnthreadedTests()
// {
// }

// void IndexManagerTester::runThreadedTests()
// {
// }

void IndexManagerTester::addAndCount()
{
    CPPUNIT_ASSERT_MESSAGE("writer == NULL", writer);
/*    if (writer == 0)
        return;*/
    
    writer->deleteAllEntries();
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { Strigi::AnalysisResult idx(s, 0, *writer, *si); }
        str.str("");
    }
    writer->commit();
    CPPUNIT_ASSERT_ASSERTION_PASS (CPPUNIT_ASSERT(reader));
    int n = reader->countDocuments();

    str.str("");
    if (n != m) {
        str << n << " != " << m;
    }
    CPPUNIT_ASSERT_MESSAGE(str.str(), n == m);
}

void IndexManagerTester::testNumberQuery()
{
    CPPUNIT_ASSERT_MESSAGE("writer == NULL", writer);
/*    if (writer == 0)
        return;*/
    
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
            Strigi::AnalysisResult idx(name, 0, *writer, *si);
            idx.addValue(idx.config().fieldRegister().sizeField, value);
        }
        str.str("");
    }
    writer->commit();
    Strigi::QueryParser parser;

    //TODO: ask Jos if it is correct
    // BEFORE: Strigi::Query q = parser.buildQuery("size:>0", -1, 0);
    Strigi::Query q = parser.buildQuery("size:>0");
    int count = reader->countHits(q);
    
    char buff [100];
    if (count != m)
        snprintf (buff, 100 * sizeof (char), "%i != %i", count, m);
    
    CPPUNIT_ASSERT_MESSAGE(buff, count == m);
}
