/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "indexwritertester.h"
#include "indexsearchtester.h"
using namespace strigiunittest;

class EstraierIndexReaderTest : public IndexReaderTest {
private:
    CPPUNIT_TEST_SUB_SUITE( EstraierIndexReaderTest, IndexReaderTest);
    CPPUNIT_TEST_SUITE_END();
public:
    EstraierIndexReaderTest() :IndexReaderTest("estraier") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( EstraierIndexReaderTest );

class EstraierIndexWriterTest : public IndexWriterTest {
private:
    CPPUNIT_TEST_SUB_SUITE( EstraierIndexWriterTest, IndexWriterTest);
    CPPUNIT_TEST_SUITE_END();
public:
    EstraierIndexWriterTest() :IndexWriterTest("estraier") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( EstraierIndexWriterTest );


class EstraierIndexSearchTest : public IndexSearchTest {
private:
    CPPUNIT_TEST_SUB_SUITE( EstraierIndexSearchTest, IndexSearchTest);
    CPPUNIT_TEST_SUITE_END();
public:
    EstraierIndexSearchTest() :IndexSearchTest("estraier") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( EstraierIndexSearchTest );
