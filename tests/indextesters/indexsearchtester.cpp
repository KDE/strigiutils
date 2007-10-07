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

#include "indexsearchtester.h"
#include "analyzerconfiguration.h"
#include "diranalyzer.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "query.h"
#include "queryparser.h"
#include "unittestfunctions.h"

#include  <errno.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace strigiunittest;

void
IndexSearchTester::setUp() {
    char buff[13];
    char* dirname;
    string separator;
    
    // generate index dir name
    strcpy(buff, "strigiXXXXXX");
    dirname = mkdtemp(buff);
    
    if (dirname) {
        cout << "created index dir: " << dirname << endl;
        indexdir.assign(dirname);
    } else {
        cerr << "Error creating temporary directory for index because of: "
            << strerror(errno) << endl;
        return;
    }

    // generate indexed docs name
    strcpy(buff, "strigiXXXXXX");
    dirname = mkdtemp(buff);
    if (dirname) {
        cout << "created dir for testing documents: " << dirname << endl;
        filedir.assign(dirname);
    } else {
        cerr << "Error creating temporary directory for indexed docs: "
            << strerror(errno) << endl;
        return;
    }
    
#ifdef _WIN32
    separator = "\\";
#else
    separator = "/";
#endif

    // prepare files to be indexed
    string filename;
    string filecontents;
    
    filename = "testfile01";
    filecontents = "this is a simple test file";
    indexedFiles.insert(make_pair<string, string> (filename, filecontents));
    
    filename = "testfile02";
    filecontents = "unit testing example";
    indexedFiles.insert(make_pair<string, string> (filename, filecontents));
    
    // create files on file system
    for (map<string,string>::iterator iter = indexedFiles.begin();
            iter != indexedFiles.end(); iter++) {
        ofstream file;
        string fullpath = filedir + separator + iter->first;
        file.open( fullpath.c_str());
        if (file.is_open()) {
            file << iter->second;
            file.close();
        } else {
            cerr << "error during creation of file " << fullpath << endl;
        }
    }
    
    manager = getIndexManager(backend, indexdir);

    CPPUNIT_ASSERT_MESSAGE ("manager == null", manager);
    
    Strigi::AnalyzerConfiguration config;
    Strigi::StreamAnalyzer* streamindexer = new Strigi::StreamAnalyzer(config);
    Strigi::IndexWriter* writer = manager->indexWriter();
    streamindexer->setIndexWriter(*writer);
 
    for (map<string,string>::iterator iter = indexedFiles.begin();
         iter != indexedFiles.end(); iter++) {
        string temp = filedir + separator + iter->first;
        fprintf (stderr, "going to index %s\n", temp.c_str());
        streamindexer->indexFile(temp);
    }
    
    delete streamindexer;
    
    writer->commit();
    
}

void
IndexSearchTester::tearDown() {
    if (manager)
        delete manager;
    manager = NULL;
    
    // clean up data (does not work on windows)
    string cmd = "rm -r " + indexdir + " " + filedir;
    system(cmd.c_str());
}

void
IndexSearchTester::testVariables() {
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("backend empty", !backend.empty());
    CPPUNIT_ASSERT_MESSAGE ("indexdir empty", !indexdir.empty());
    CPPUNIT_ASSERT_MESSAGE ("filedir empty", !filedir.empty());
    
    unsigned int indexedFilesSize = manager->indexReader()->countDocuments();
    if (indexedFilesSize != indexedFiles.size()) {
        ostringstream msg;
        msg << "There are " << indexedFilesSize << " indexed files instead of "
            << indexedFiles.size();
        CPPUNIT_FAIL(msg.str());
    }
}

void
IndexSearchTester::testSystemLocationSearchIndexedFile() {
    Strigi::IndexReader* reader = manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader == NULL", reader);
    
    Strigi::QueryParser parser;

    Strigi::Query query = parser.buildQuery("name:'testfile01'");
    vector<Strigi::IndexedDocument> matches = reader->query(query, 0, 10);

    int nhits = matches.size();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of hits is wrong.", nhits, 1);
}

void
IndexSearchTester::testSystemLocationSearchUnindexedFile() {
    Strigi::IndexReader* reader = manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader == NULL", reader);

    Strigi::QueryParser parser;

    Strigi::Query query = parser.buildQuery(
        Strigi::FieldRegister::pathFieldName+":'unindexed'");
    vector<Strigi::IndexedDocument> matches = reader->query(query, 0, 10);
    int nhits = matches.size();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of hits is wrong.", nhits, 0);
}

