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

#include <algorithm>
#include <fstream>
#include <set>

#include <unistd.h>

/**
 * This file misses documentation on how it works and what it does.
 **/

using namespace std;
using namespace strigiunittest;

namespace strigiunittest
{
    class FindFilterRule {
        private:
            string m_rule;
            bool   m_value;
    
        public:
            FindFilterRule(bool value, string rule) {
                m_value = value;
                m_rule = rule;
            }
    
            bool operator()(pair<bool,string> filterRule) {
                if (filterRule.first != m_value)
                    return false;
                
                if (m_rule.compare(filterRule.second) != 0)
                    return false;

                return true;
            }
    };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DaemonConfiguratorTest );

/*!
* Test the equivalence of two different set of strings
*/
bool DaemonConfiguratorTest::checkEq(set<string>& ori, set<string>& last, 
                                     string& errMsg)
{
    for (set<string>::iterator itOri = ori.begin(); itOri != ori.end(); itOri++)
    {
        set<string>::iterator itLast = last.find(*itOri);
        if (itLast == last.end())
        {
            errMsg = *itOri;
            errMsg += " was not found\n";
            return false;
        }
    }
    
    if (ori.size() == last.size()) {
        return true;
    } else {
        ostringstream str;
        str << "Found different size (" << ori.size() << " != " << last.size()
            << ")" << endl;
        errMsg += str.str();
        return false;
    }
}

/*!
 * Test the equivalence of two different set of filtering rules
 */
bool DaemonConfiguratorTest::checkEq(Strigi::AnalyzerConfiguration& oriConf,
                                     Strigi::AnalyzerConfiguration& lastConf,
                                     string& errMsg)
{
    vector<pair<bool,string> > filtersOri, filtersLast;
    
    // retrieve rules
    filtersOri  = oriConf.filters();
    filtersLast = lastConf.filters();
    
    // check retrieved values
    for (vector<pair<bool,string> >::iterator itOri = filtersOri.begin();
         itOri != filtersOri.end(); itOri++)
    {
        FindFilterRule findRule (itOri->first, itOri->second);
        
        vector<pair<bool,string> >::iterator itLast = find_if ( 
                                                            filtersLast.begin(),
                                                            filtersLast.end(),
                                                            findRule);
        if (itLast == filtersLast.end())
        {
            errMsg = "filtering rule regarding ";
            errMsg += itOri->second;
            errMsg += " was not found\n";
            return false;
        }
    }
    
    if (filtersOri.size() == filtersLast.size()) {
        return true;
    } else {
        ostringstream str;
        str << "Found different size (" << filtersOri.size() << " != "
            << filtersLast.size() << ")" << endl;
        errMsg += str.str();
        return false;
    }
}

void DaemonConfiguratorTest::setUp()
{
    //touch file
    confFile1 = "StrigiDaemonConfiguratorTest.tmp";
    ofstream file (confFile1.c_str(), ios::binary|ios::trunc);
    file.close();
    defaultConf = new DaemonConfigurator(confFile1);
    
    confFile2 = "StrigiDaemonConfiguratorTestSaved.tmp";
    savedConf   = NULL;
}

void DaemonConfiguratorTest::tearDown()
{
    int ret;
    string err;
    
    delete defaultConf;

    if (savedConf != NULL)
    {
        delete savedConf;
        
        ret = unlink(confFile2.c_str());
    
        if (ret == -1)
        {
            err = "Error while removing temp file ";
            err += confFile2;
            perror (err.c_str());
        }
    }
    
    ret = unlink(confFile1.c_str());
    
    if (ret == -1)
    {
        err = "Error while removing temp file ";
        err += confFile1;
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
    CPPUNIT_ASSERT (anConf.filters().size() > 0);
}

void DaemonConfiguratorTest::testIndexedDirs()
{
    set<string> dirsSet, dirsGet, dirsDelta, dirsLast;
    bool equal;
    string errMsg;
    
    dirsSet.insert ("/foo/bar");
    dirsSet.insert ("/bar");
    
    // override default indexed dirs
    defaultConf->setIndexedDirectories( dirsSet, "localhost", false);
    
    dirsGet = defaultConf->getIndexedDirectories("localhost");
    
    // check retrieved dirs
    equal = checkEq(dirsSet, dirsGet, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
    
    // add new dirs with merge enabled
    dirsDelta.insert("/tmp");
    defaultConf->setIndexedDirectories( dirsDelta, "localhost", true);
    
    dirsLast = dirsSet;
    for (set<string>::iterator it = dirsDelta.begin(); it != dirsDelta.end();
         it++)
    {
        dirsLast.insert(*it);
    }
    
    dirsGet.clear();
    dirsGet = defaultConf->getIndexedDirectories("localhost");
    
    // check retrieved dirs
    equal = checkEq(dirsLast, dirsGet, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
}

void DaemonConfiguratorTest::testFilters()
{
    vector<pair<bool,string> > filtersSet, filtersGet;
    
    filtersSet.push_back (make_pair(true, string("/foo")));
    filtersSet.push_back (make_pair(false, string("bar*")));
    
    // set custom filtering rules
    defaultConf->saveFilteringRules( filtersSet);
    
    // retrieve rules
    Strigi::AnalyzerConfiguration anConf;
    defaultConf->loadFilteringRules(anConf);
    filtersGet = anConf.filters();
    
    // check retrieved values
    bool check = true;
    for (vector<pair<bool,string> >::iterator itSet = filtersSet.begin();
         itSet != filtersSet.end(); itSet++)
    {
        FindFilterRule findRule (itSet->first, itSet->second);
        
        vector<pair<bool,string> >::iterator itGet = find_if (
                                                            filtersGet.begin(),
                                                            filtersGet.end(),
                                                            findRule);
        if (itGet == filtersGet.end())
        {
            check = false;
            break;
        }
    }
    
    CPPUNIT_ASSERT (check);
    CPPUNIT_ASSERT (filtersSet.size() == filtersGet.size());
}

void DaemonConfiguratorTest::testRepository()
{
    bool equal;
    string errMsg;
    
    // retrieve directories associated to a non-existing repository
    set<string> dirs = defaultConf->getIndexedDirectories( "newRepository");
    
    CPPUNIT_ASSERT (dirs.size() == 0);
    
    // save the number of directories associated to the localhost repository
    set<string> localhostRepoEntriesIni = defaultConf->getIndexedDirectories();
    CPPUNIT_ASSERT (localhostRepoEntriesIni.size() > 0);
    
    
    // add new dir to newRepository list, overwrite old dir associated to this
    // repository
    set<string> setDirs;
    setDirs.insert ("/foobar");
    defaultConf->setIndexedDirectories( setDirs, "newRepository", false);
    dirs = defaultConf->getIndexedDirectories( "newRepository");
    equal = checkEq (setDirs, dirs, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
    
    // default repository dirs must be the same
    set<string> localhostRepoEntries = defaultConf->getIndexedDirectories();
    equal = checkEq (localhostRepoEntriesIni, localhostRepoEntries, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
}

void DaemonConfiguratorTest::testPollingInterval()
{
    char buffer [10];
    int ret;
    string message;

    // test polling interval configuration for default repository
    int defRepVal= DaemonConfigurator::DEFAULT_POLLING_INTERVAL + 1;
    defaultConf->setPollingInterval( defRepVal);
    ret = defaultConf->getPollingInterval();

    if (ret != defRepVal)
    {
        message = "Failed to set polling interval for default repository: set ";
        snprintf (buffer, 10, "%i", defRepVal);
        message += buffer;
        message += ", got ";
        snprintf (buffer, 10, "%i", ret);
        message += buffer;
        CPPUNIT_FAIL (message);
    }

    // test polling interval configuration for another repository
    int newRepVal= DaemonConfigurator::DEFAULT_POLLING_INTERVAL + 10;
    defaultConf->setPollingInterval( newRepVal, "newRepository");
    ret = defaultConf->getPollingInterval("newRepository");

    if (ret != newRepVal)
    {
        message = "Failed to set polling interval for \"newRepository\"";
        message += "repository: set ";
        snprintf (buffer, 10, "%i", newRepVal);
        message += buffer;
        message += ", got ";
        snprintf (buffer, 10, "%i", ret);
        message += buffer;
        CPPUNIT_FAIL (message);
    }

    // polling interval associated to the default repository
    // should not have been changed
    ret = defaultConf->getPollingInterval();

    if (ret != defRepVal)
    {
        message = "After configuration of polling interval for \"newRepository\"";
        message += "repository, default repository polling interval changed!\n";
        message += "Expected ";
        snprintf (buffer, 10, "%i", defRepVal);
        message += buffer;
        message += ", got ";
        snprintf (buffer, 10, "%i", ret);
        message += buffer;
        CPPUNIT_FAIL (message);
    }
    
    // test minimum polling interval value
    defaultConf->setPollingInterval( 0);
    ret = defaultConf->getPollingInterval();
    if (ret != DaemonConfigurator::DEFAULT_POLLING_INTERVAL)
    {
        message = "Test minimum polling interval failed, expected ";
        snprintf (buffer, 10,"%i",DaemonConfigurator::DEFAULT_POLLING_INTERVAL);
        message += buffer;
        message += ", got ";
        snprintf (buffer, 10, "%i", ret);
        message += buffer;
        CPPUNIT_FAIL (message);
    }
}

void DaemonConfiguratorTest::testSave()
{
    bool equal;
    string errMsg;
    
    // save defaultConf to disk
    defaultConf->save(confFile2.c_str());
    
    // load saved conf
    savedConf = new DaemonConfigurator(confFile2);
    
    // compare filtering rules
    Strigi::AnalyzerConfiguration anConfDefault;
    defaultConf->loadFilteringRules(anConfDefault);
    
    Strigi::AnalyzerConfiguration anConfSaved;
    savedConf->loadFilteringRules(anConfSaved);
    
    equal = checkEq (anConfDefault, anConfSaved, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
    
    // compare repositories
    set<string> localhostRepoEntriesDef = defaultConf->getIndexedDirectories();
    set<string> localhostRepoEntriesSaved = savedConf->getIndexedDirectories();
    set<string> newRepoEntriesDef =
                            defaultConf->getIndexedDirectories("newRepository");
    set<string> newRepoEntriesSaved =
                              savedConf->getIndexedDirectories("newRepository");
    
    equal = checkEq(localhostRepoEntriesDef, localhostRepoEntriesSaved, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
    
    equal = checkEq(newRepoEntriesDef, newRepoEntriesSaved, errMsg);
    CPPUNIT_ASSERT_MESSAGE (errMsg, equal);
    
    // compare polling interval
    int localhostRepoPollingDef = defaultConf->getPollingInterval();
    int localhostRepoPollingSaved = savedConf->getPollingInterval();
    int newRepoPollingDef = defaultConf->getPollingInterval("newRepository");
    int newRepoPollingSaved = savedConf->getPollingInterval("newRepository");
    
    CPPUNIT_ASSERT (localhostRepoPollingDef == localhostRepoPollingSaved);
    CPPUNIT_ASSERT (newRepoPollingDef == newRepoPollingSaved);
}
