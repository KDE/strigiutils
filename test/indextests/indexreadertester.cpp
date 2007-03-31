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
 
/*#include "analysisresult.h"
#include "analyzerconfiguration.h"*/
#include "indexmanager.h"
// #include "indexwriter.h"
#include "indexreader.h"
// #include "query.h"

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

void IndexReaderTester::setUp()
{
    manager = NULL;
    reader  = NULL;
}

void IndexReaderTester::tearDown()
{
    delete manager;
    reader = NULL;
}

void IndexReaderTester::testVariables()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("reader  == NULL", reader);
/*    CPPUNIT_ASSERT_MESSAGE ("si == NULL", si);
    CPPUNIT_ASSERT_MESSAGE ("ic == NULL", ic);*/
}

void IndexReaderTester::getFiles()
{
    CPPUNIT_ASSERT_MESSAGE("reader == NULL", reader);
    
    reader->files(0);
    //TODO: think to a better assert
}
