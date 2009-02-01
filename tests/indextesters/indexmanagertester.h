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
#ifndef UNIT_TEST_INDEX_MANAGER_TESTER_H
#define UNIT_TEST_INDEX_MANAGER_TESTER_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace Strigi {
    class IndexManager;
    class IndexWriter;
    class IndexReader;
}

namespace strigiunittest {

class IndexManagerTest : public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE( IndexManagerTest );
    CPPUNIT_TEST( testIndexManagerOnInvalidDirectory );
    CPPUNIT_TEST( testIndexReader );
    CPPUNIT_TEST( testIndexWriter );
    CPPUNIT_TEST_SUITE_END_ABSTRACT();

    const std::string m_backendname;
    const std::string m_indexpath;
    Strigi::IndexManager* m_manager;

protected:
    IndexManagerTest(const std::string& backendname);

public:
    void setUp();
    void tearDown();

    void testIndexReader();
    void testIndexWriter();
    void testIndexManagerOnInvalidDirectory();
};

class CLuceneIndexManagerTest : public IndexManagerTest {
private:
    CPPUNIT_TEST_SUB_SUITE( CLuceneIndexManagerTest, IndexManagerTest);
    CPPUNIT_TEST_SUITE_END();
public:
    CLuceneIndexManagerTest() :IndexManagerTest("clucene") {}
};

}
#endif
