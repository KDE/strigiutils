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
#ifndef UNIT_TEST_INDEX_READER_TESTER_H
#define UNIT_TEST_INDEX_READER_TESTER_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "analyzerconfiguration.h"
#include "fieldtypes.h"

namespace Strigi {
    class IndexManager;
    class IndexReader;
    class IndexWriter;
    class StreamAnalyzer;
}

namespace strigiunittest
{
    class IndexReaderTester : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( IndexReaderTester );
        CPPUNIT_TEST( testChildrenRetrieval );
        CPPUNIT_TEST( addAndCount );
        CPPUNIT_TEST( testNumberQuery );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();
        
    private:
	Strigi::IndexManager* m_manager;

	Strigi::IndexWriter* m_writer;
	Strigi::IndexReader* m_reader;
	Strigi::StreamAnalyzer* m_streamAnalyzer;
	Strigi::AnalyzerConfiguration m_analyzerConfiguration;
	Strigi::FieldRegister m_fieldRegister;

    protected:
	virtual Strigi::IndexManager* createManager() = 0;
	/**
	 * delete the manager. The default implementation simply
	 * calls delete.
	 */
	virtual void deleteManager( Strigi::IndexManager* );

    public:
	virtual void setUp();
	virtual void tearDown();
            
	virtual void addAndCount();
	virtual void testChildrenRetrieval();
	virtual void testNumberQuery();
    };
}

#endif
