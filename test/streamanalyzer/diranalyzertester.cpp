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
#include "analyzerconfiguration.h"
#include "diranalyzer.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "unittestfunctions.h"

#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace strigiunittest;

void DirAnalyzerTester::setUp()
{
    char* temp;
    char* dir = "tmp";
    char* prefix = "strigi";
    
    // generate index name
    temp = tempnam (dir, prefix);
    indexdir = temp;
    free (temp);
    
    // initialize a directory for writing and an indexmanager
#ifdef _WIN32
    mkdir(indexdir.c_str());
    separator = "\\";
#else
    mkdir(indexdir.c_str(), S_IRUSR|S_IWUSR|S_IXUSR);
    separator = "/";
#endif
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
        ofstream file;
        file.open( iter->first.c_str());
        if (file.is_open())
        {
            file << iter->second;
            file.close();
        }
        else
            fprintf (stderr, "errore nella creazione del file\n");
    }
    
    manager = getIndexManager(backend, indexdir);
}

void DirAnalyzerTester::tearDown()
{
    if (manager)
        delete manager;
    manager = NULL;
    
    // clean up data
    string cmd = "rm -r ";
    cmd += indexdir;
    system(cmd.c_str());
}

void DirAnalyzerTester::testVariables()
{
    CPPUNIT_ASSERT_MESSAGE ("manager == NULL", manager);
    CPPUNIT_ASSERT_MESSAGE ("backend empty", !backend.empty());
    CPPUNIT_ASSERT_MESSAGE ("indexdir empty", !indexdir.empty());
    
    unsigned int indexedFilesSize = manager->indexReader()->files(0).size();
    if (indexedFilesSize != indexedFiles.size())
    {
        char buffer [50];
        
        snprintf (buffer, 50*sizeof(char), "%i", indexedFilesSize);
        
        string message = "There're ";
        message += buffer;
        message += " indexed files instead of ";
        
        snprintf (buffer, 50*sizeof(char), "%i", indexedFiles.size());
        message += buffer;
        
        CPPUNIT_FAIL (message);
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
        string temp = indexdir + separator + iter->first;
        fprintf (stderr, "going to index %s\n", temp.c_str());
        analyzer->analyzeDir (indexdir + separator + iter->first, 1);
    }
    
    delete analyzer;
    
    unsigned int indexedFilesSize = manager->indexReader()->files(0).size();
    if (indexedFilesSize != indexedFiles.size())
    {
        char buffer [50];
        
        snprintf (buffer, 50*sizeof(char), "%i", indexedFilesSize);
        
        string message = "There're ";
        message += buffer;
        message += " indexed files instead of ";
        
        snprintf (buffer, 50*sizeof(char), "%i", indexedFiles.size());
        message += buffer;
        
        CPPUNIT_FAIL (message);
    }
}

