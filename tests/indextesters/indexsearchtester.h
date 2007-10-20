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
#ifndef UNIT_TEST_INDEX_SEARCH_TESTER_H
#define UNIT_TEST_INDEX_SEARCH_TESTER_H

#include "indextest.h"

#include <map>

namespace strigiunittest {

class IndexSearchTest : public IndexTest {
private:
    CPPUNIT_TEST_SUITE( IndexSearchTest );
    CPPUNIT_TEST( testVariables );
    CPPUNIT_TEST( testSystemLocationSearchIndexedFile );
    CPPUNIT_TEST( testSystemLocationSearchUnindexedFile );
    CPPUNIT_TEST_SUITE_END_ABSTRACT();

    std::string filedir;
    std::map<std::string,std::string> indexedFiles; //!< map with key = file name, and value = file contents

    void testSystemLocationSearchIndexedFile();
    void testSystemLocationSearchUnindexedFile();
    void testVariables();

public:
    IndexSearchTest(const std::string& backendname) :IndexTest(backendname){}
    void setUp();
    void tearDown();
};

}

#endif
