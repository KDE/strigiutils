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

class CLuceneIndexReaderTest : public IndexReaderTest {
private:
    CPPUNIT_TEST_SUB_SUITE( CLuceneIndexReaderTest, IndexReaderTest);
    CPPUNIT_TEST_SUITE_END();
public:
    CLuceneIndexReaderTest() :IndexReaderTest("clucene") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( CLuceneIndexReaderTest );

class CLuceneIndexWriterTest : public IndexWriterTest {
private:
    CPPUNIT_TEST_SUB_SUITE( CLuceneIndexWriterTest, IndexWriterTest);
    CPPUNIT_TEST_SUITE_END();
public:
    CLuceneIndexWriterTest() :IndexWriterTest("clucene") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( CLuceneIndexWriterTest );

class CLuceneIndexSearchTest : public IndexSearchTest {
private:
    CPPUNIT_TEST_SUB_SUITE( CLuceneIndexSearchTest, IndexSearchTest);
    CPPUNIT_TEST_SUITE_END();
public:
    CLuceneIndexSearchTest() :IndexSearchTest("clucene") {}
};
CPPUNIT_TEST_SUITE_REGISTRATION( CLuceneIndexSearchTest );

