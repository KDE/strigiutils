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
#include "fieldtypes.h"
#include "query.h"
#include "queryparser.h"
#include "indexpluginloader.h"

#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;
using namespace Strigi;
using namespace strigiunittest;

void IndexManagerTester::setUp()
{
    m_manager = createManager();
}

void IndexManagerTester::tearDown()
{
    deleteManager( m_manager );
}


void IndexManagerTester::deleteManager( Strigi::IndexManager* m )
{
    Strigi::IndexPluginLoader::deleteIndexManager(m);
}


void IndexManagerTester::testIndexReader()
{
    Strigi::IndexReader* reader = m_manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", reader);
}

void IndexManagerTester::testIndexWriter()
{
    Strigi::IndexWriter* writer = m_manager->indexWriter();
    CPPUNIT_ASSERT_MESSAGE("writer creation failed", writer);
}
