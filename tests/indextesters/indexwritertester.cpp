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
#include "indexwritertester.h"
 
#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "indexmanager.h"
#include "indexwriter.h"
#include "query.h"

#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;
using namespace strigiunittest;

void IndexWriterTester::setUp()
{
    manager = NULL;
    writer  = NULL;
    si      = NULL;
    ic      = NULL;
}

void IndexWriterTester::tearDown()
{
    delete manager;
    delete si;
    delete ic;
    writer = NULL;
}

void IndexWriterTester::testVariables()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("writer  == NULL", writer);
    CPPUNIT_ASSERT_MESSAGE ("si == NULL", si);
    CPPUNIT_ASSERT_MESSAGE ("ic == NULL", ic);
}

void IndexWriterTester::add()
{
    CPPUNIT_ASSERT_MESSAGE("writer == NULL", writer);
    
    std::string s("a"); // we must pass a string, not a const char*
    Strigi::AnalysisResult i(s, 0, *writer, *si);

    writer->commit();
}

void IndexWriterTester::optimize()
{
    CPPUNIT_ASSERT_MESSAGE("writer == NULL", writer);
    
    writer->optimize();
}
