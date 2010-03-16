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

#include <cstdlib>

#include "indextest.h"
#include <strigi/indexmanager.h>
#include <strigi/indexpluginloader.h>

using namespace std;
using namespace Strigi;
using namespace strigiunittest;

IndexTest::IndexTest(const string& backendname, const string& indexpath)
    :m_backendname(backendname), m_indexpath(":memory:"), m_manager(0) {
}

void
IndexTest::setUp() {
    m_manager = IndexPluginLoader::createIndexManager(
        m_backendname.c_str(), m_indexpath.c_str());
    CPPUNIT_ASSERT_MESSAGE("manager creation failed", m_manager);
    if (!m_manager) return;

    m_writer = m_manager->indexWriter();
    CPPUNIT_ASSERT_MESSAGE("writer creation failed", m_writer);

    m_reader = m_manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", m_reader);
}

void
IndexTest::tearDown() {
    IndexPluginLoader::deleteIndexManager(m_manager);
    // clean up data (if any)
    string cmd("rm -rf '" + m_indexpath + "'");
    int r = system(cmd.c_str());
     CPPUNIT_ASSERT_MESSAGE("cleanup failed", r == 0);
}
