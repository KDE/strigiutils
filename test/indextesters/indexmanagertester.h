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

#include <TestFixture.h>
#include <extensions/HelperMacros.h>
#include <string>

namespace Strigi {
    class IndexManager;
    class IndexWriter;
    class IndexReader;
    class StreamAnalyzer;
    class AnalyzerConfiguration;
}

namespace strigiunittest
{
    class IndexManagerTester : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( IndexManagerTester );
        CPPUNIT_TEST( testVariables );
        CPPUNIT_TEST( addAndCount );
        CPPUNIT_TEST( testNumberQuery );
        //CPPUNIT_TEST( runUnthreadedTests );
        //CPPUNIT_TEST( runThreadedTests );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();
        
        protected:
            //StrigiMutex lock;
            Strigi::IndexManager* manager;
            Strigi::IndexWriter* writer;
            Strigi::IndexReader* reader;
            Strigi::StreamAnalyzer* si;
            Strigi::AnalyzerConfiguration* ic;
            int m;
            std::string path;
        
        public:
            virtual void setUp();
            virtual void tearDown();
            
//             virtual void runUnthreadedTests();
//             virtual void runThreadedTests();
            virtual void addAndCount();
            virtual void testNumberQuery();
            virtual void testVariables();
    };
}

#endif
