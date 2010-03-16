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
#ifndef UNIT_TEST_INDEX_WRITER_TESTER_H
#define UNIT_TEST_INDEX_WRITER_TESTER_H

#include "indextest.h"

#include <strigi/analyzerconfiguration.h>
#include <strigi/fieldtypes.h>

namespace Strigi {
    class StreamAnalyzer;
    class AnalysisResult;
}

namespace strigiunittest {

class IndexWriterTest : public IndexTest {
private:
    CPPUNIT_TEST_SUITE( IndexWriterTest );
    CPPUNIT_TEST( testAddText );
    CPPUNIT_TEST( testDeleteAllEntries );
    CPPUNIT_TEST( testDeleteNestedEntries );
    CPPUNIT_TEST_SUITE_END_ABSTRACT();

    Strigi::StreamAnalyzer* m_streamAnalyzer;
    Strigi::AnalyzerConfiguration m_analyzerConfiguration;
    Strigi::FieldRegister m_fieldRegister;

    void testAddText();
    void testDeleteAllEntries();
    void testDeleteEntries();
    void testDeleteNestedEntries();

public:
    IndexWriterTest(const std::string& backendname) :IndexTest(backendname),
        m_streamAnalyzer(0) {}
    void setUp();
    void tearDown();
};

}

#endif
