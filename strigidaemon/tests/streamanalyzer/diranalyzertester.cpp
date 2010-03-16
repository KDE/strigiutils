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

#include "diranalyzertester.h"
#include <strigi/analyzerconfiguration.h>
#include <strigi/diranalyzer.h>
#include <strigi/indexmanager.h>
#include <strigi/indexreader.h>
#include <strigi/indexpluginloader.h>
#include "utils/unittestfunctions.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;
using namespace strigiunittest;

#ifdef _WIN32
    const string DirAnalyzerTester::separator = "\\";
#else
    const string DirAnalyzerTester::separator = "/";
#endif

void DirAnalyzerTester::setUp() {
    char buff[13];
    char* dirname;

    // generate index dir name
    strcpy(buff, "strigiXXXXXX");
    dirname = mkdtemp(buff);

    if (dirname == NULL) {
        cerr << "Error creating temporary directory for index because of: "
            << strerror(errno) << endl;
        return;
    } else {
        //cout << "created index dir: " << dirname << endl;
        indexdir.assign(dirname);
    }

    // generate indexed docs name
    strcpy(buff, "strigiXXXXXX");
    dirname = mkdtemp(buff);

    if (dirname == NULL) {
        cerr << "Error creating temporary directory for indexed docs: "
            << strerror(errno) << endl;
        return;
    } else {
        //cout << "created dir for testing documents: " << dirname << endl;
        filedir.assign(dirname);
    }

    // prepare files to be indexed
    string filename;
    string filecontents;

    filename = "testfile01";
    filecontents = "this is a simple test file";
    indexedFiles.insert (make_pair<string, string> (filename, filecontents));

    filename = "testfile02";
    filecontents = "unit testing example";
    indexedFiles.insert (make_pair<string, string> (filename, filecontents));

    // create files on file system
    for (map<string,string>::iterator iter = indexedFiles.begin();
         iter != indexedFiles.end(); iter++)
    {
        string fullpath = filedir + separator + iter->first;

        ofstream file;
        file.open(fullpath.c_str());
        if (file.is_open()) {
            file << iter->second;
            file.close();
        } else {
            cerr << "error during creation of file " << fullpath;
        }
    }

    manager = getIndexManager(backend, indexdir);
}

void DirAnalyzerTester::tearDown()
{
    if (manager) {
        Strigi::IndexPluginLoader::deleteIndexManager(manager);
    }
    manager = NULL;

    // clean up data
    string cmd = "rm -r ";
    cmd += indexdir;
    cmd += " ";
    cmd += filedir;
    int r = system(cmd.c_str());
    CPPUNIT_ASSERT_MESSAGE("cleanup failed", r == 0);
}

void DirAnalyzerTester::testVariables()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("backend empty", !backend.empty());
    CPPUNIT_ASSERT_MESSAGE ("indexdir empty", !indexdir.empty());

    // THIS TEST IS BROKEN: you cannot expect documents without indexing them
    unsigned int indexedFilesSize = manager->indexReader()->countDocuments();
    if (indexedFilesSize != indexedFiles.size()) {
        ostringstream msg;
        msg << "There are " << indexedFilesSize << " indexed files instead of "
            << indexedFiles.size();
        //CPPUNIT_FAIL(msg.str());
    }
}

void DirAnalyzerTester::testCreateIndex()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == null", manager);

    Strigi::AnalyzerConfiguration config;
    Strigi::DirAnalyzer* analyzer = new Strigi::DirAnalyzer(*manager, config);

    for (map<string,string>::iterator iter = indexedFiles.begin();
         iter != indexedFiles.end(); iter++)
    {
        string path = filedir + separator + iter->first;
        // cerr << "going to index " << path << endl;
        int retval = analyzer->analyzeDir(path, 1);
        CPPUNIT_ASSERT_MESSAGE("Error indexing "+path, retval == 0);
    }
    delete analyzer;

    CPPUNIT_ASSERT_MESSAGE("Not all documents were indexed.",
        manager->indexReader()->countDocuments()
            == (int32_t)indexedFiles.size());
    unsigned int indexedFilesSize = manager->indexReader()->countDocuments();
    if (indexedFilesSize != indexedFiles.size()) {
        ostringstream msg;
        msg << "There are " << indexedFilesSize << " indexed files instead of "
            << indexedFiles.size();
        CPPUNIT_FAIL(msg.str());
    }
}

