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
#include "luceneindexmanagertest.h"

#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "cluceneindexmanager.h"
#include "indexwriter.h"
#include "indexreader.h"
#include "query.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LuceneIndexManagerTest );

void LuceneIndexManagerTest::setUp()
{
    IndexManagerTester::setUp();
    
    path = "testcluceneindex";

    // initialize a directory for writing and an indexmanager
#ifdef _WIN32
    mkdir(path.c_str());
#else
    mkdir(path.c_str(), S_IRUSR|S_IWUSR|S_IXUSR);
#endif

    manager = createCLuceneIndexManager(path.c_str());
    writer = manager->indexWriter();
    reader = manager->indexReader();
    ic = new Strigi::AnalyzerConfiguration ();
    si = new Strigi::StreamAnalyzer (*ic);
}

void LuceneIndexManagerTest::tearDown()
{
    IndexManagerTester::tearDown();
    
    // clean up data
    string cmd = "rm -r ";
    cmd += path;
    system(cmd.c_str());
}
