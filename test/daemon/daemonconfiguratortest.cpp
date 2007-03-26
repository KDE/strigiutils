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

#include "daemonconfiguratortest.h"
#include "daemonconfigurator.h"

#include <fstream>
#include <set>
#include <unistd.h>

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DaemonConfiguratorTest );

void DaemonConfiguratorTest::setUp()
{
    //touch file
    confFile = "StrigiDaemonConfiguratorTest.tmp";
    ofstream file (confFile.c_str(), ios::binary|ios::trunc);
    file.close();
    defaultConf = new DaemonConfigurator(confFile);
}

void DaemonConfiguratorTest::tearDown()
{
    delete defaultConf;

    int ret = unlink(confFile.c_str());
    
    if (ret == -1)
    {
        string err = "Error while removing temp file ";
        err += confFile;
        perror (err.c_str());
    }
}

void DaemonConfiguratorTest::testDefaultConf()
{
    CPPUNIT_ASSERT (defaultConf);
    
    // indexed dirs for localhost repository must be different from zero
    CPPUNIT_ASSERT (defaultConf->getIndexedDirectories().size() > 0);
    
    // test default filtering rules presence
    Strigi::AnalyzerConfiguration anConf;
    defaultConf->loadFilteringRules(anConf);
    CPPUNIT_ASSERT (anConf.getFilters().size() > 0);
}
