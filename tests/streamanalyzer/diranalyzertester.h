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
#ifndef UNIT_TEST_DIRANALYZER_TEST_H
#define UNIT_TEST_DIRANALYZER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <map>
#include <string>

namespace Strigi
{
    class IndexManager;
}

namespace strigiunittest
{
    class DirAnalyzerTester : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( DirAnalyzerTester );
        CPPUNIT_TEST( testVariables );
        CPPUNIT_TEST( testCreateIndex );
        CPPUNIT_TEST_SUITE_END_ABSTRACT();
        
        protected:
            Strigi::IndexManager* manager;
            std::string backend;
            std::string indexdir;
            std::string filedir;
            static const std::string separator;
            std::map<std::string,std::string> indexedFiles; //!< map with key = file name, and value = file contents

        public:
            virtual void setUp();
            virtual void tearDown();
            
            virtual void testCreateIndex();
            virtual void testVariables();
    };
}

#endif
