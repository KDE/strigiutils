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
#include <strigi/indexmanager.h>
#include <strigi/indexwriter.h>
#include <strigi/indexpluginloader.h>

#include <stdlib.h>

using namespace std;
using namespace Strigi;
using namespace strigiunittest;

CPPUNIT_TEST_SUITE_REGISTRATION( CLuceneIndexManagerTest );

IndexManagerTest::IndexManagerTest(const std::string& backendname)
    :m_backendname(backendname), m_indexpath(":memory:"), m_manager(0) {
}

void
IndexManagerTest::setUp() {
    m_manager = Strigi::IndexPluginLoader::createIndexManager(
        m_backendname.c_str(), m_indexpath.c_str());
}

void
IndexManagerTest::tearDown() {
    Strigi::IndexPluginLoader::deleteIndexManager(m_manager);
    // clean up data (if any)
    string cmd("rm -rf '" + m_indexpath + "'");
    int r = system(cmd.c_str());
    CPPUNIT_ASSERT_MESSAGE("cleanup failed", r == 0);
}

void
IndexManagerTest::testIndexReader() {
    CPPUNIT_ASSERT_MESSAGE("no manager available", m_manager);
    Strigi::IndexReader* reader = m_manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", reader);
}

void
IndexManagerTest::testIndexWriter() {
    CPPUNIT_ASSERT_MESSAGE("no manager available", m_manager);
    Strigi::IndexWriter* writer = m_manager->indexWriter();
    CPPUNIT_ASSERT_MESSAGE("writer creation failed", writer);
}
void
IndexManagerTest::testIndexManagerOnInvalidDirectory() {
    // Creating an index manager on a non exitent directory should not cause
    // major problems. Because in CLucene this can nevertheless lead to
    // 'unknown errors' we have this test. It is there to verify that
    // this code should not crash the program or throw an uncaught exception.
    const char *dir = "i am a non-existent / directory";
    Strigi::IndexManager* manager
        = Strigi::IndexPluginLoader::createIndexManager(
            m_backendname.c_str(), dir);
    Strigi::IndexPluginLoader::deleteIndexManager(manager);
}
