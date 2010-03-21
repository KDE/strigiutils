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
#ifndef UNIT_TEST_DAEMONCONFIGURATORTEST_H
#define UNIT_TEST_DAEMONCONFIGURATORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <strigi/analyzerconfiguration.h>

#include <set>
#include <string>

class DaemonConfigurator;

namespace strigiunittest
{
    class DaemonConfiguratorTest : public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( DaemonConfiguratorTest );
        CPPUNIT_TEST( testDefaultConf );
        CPPUNIT_TEST( testIndexedDirs );
        CPPUNIT_TEST( testFilters );
        CPPUNIT_TEST( testRepository );
        CPPUNIT_TEST( testPollingInterval );
        CPPUNIT_TEST_SUITE_END();
        
        private:
            std::string confFile1;
            std::string confFile2;
            DaemonConfigurator* defaultConf;
            DaemonConfigurator* savedConf;
            
            bool checkEq(std::set<std::string>&, std::set<std::string>&,
                         std::string&);
            
            bool checkEq( Strigi::AnalyzerConfiguration&,
                          Strigi::AnalyzerConfiguration&,
                          std::string&);
        
        public:
            void setUp();
            void tearDown();
            
            void testDefaultConf();
            void testIndexedDirs();
            void testFilters();
            void testRepository();
            void testPollingInterval();
            void testSave();
            
            /*
            TODO: write unit test for these methods
            std::string getWriteableIndexType() const;
            std::string getWriteableIndexDir() const;
            std::list<Repository> getReadOnlyRepositories() const;
            */
    };
}

#endif
