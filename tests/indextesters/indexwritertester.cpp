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
#include "indexwritertester.h"

#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "indexmanager.h"
#include "indexwriter.h"
#include "indexreader.h"
#include "fieldtypes.h"
#include "analyzerconfiguration.h"
#include "query.h"
#include "queryparser.h"
#include "indexpluginloader.h"

#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;
using namespace strigiunittest;
using namespace Strigi;

void IndexWriterTester::setUp()
{
    m_manager = createManager();
    m_writer = m_manager->indexWriter();
    m_reader = m_manager->indexReader();

    CPPUNIT_ASSERT_MESSAGE("writer creation failed", m_writer);
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", m_reader);

    m_streamAnalyzer = new Strigi::StreamAnalyzer( m_analyzerConfiguration );
}

void IndexWriterTester::tearDown()
{
    delete m_streamAnalyzer;
    deleteManager( m_manager );
}


void IndexWriterTester::deleteManager( Strigi::IndexManager* m )
{
    Strigi::IndexPluginLoader::deleteIndexManager(m);
}


void IndexWriterTester::testAddText()
{
    // test adding very simple text
    string path( "/tmp/dummy.txt" );
    {
        Strigi::AnalysisResult result( path, 1, *m_writer, *m_streamAnalyzer );
        std::string text( "Some dummy text for testing." );
        m_writer->addText( &result, text.c_str(), text.length() );
    }
    m_writer->commit();

    std::vector<IndexedDocument> results = m_reader->query( QueryParser::buildQuery("dummy"), 0, 100 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "got invalid number of results", 1, ( int )results.size() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "got invalid path result", path, results[0].uri );

    m_writer->deleteAllEntries();

    // FIXME: test adding unicode text

}


void IndexWriterTester::testDeleteAllEntries()
{
    // add some random data
    string path( "/tmp/dummy.txt" );
    {
        Strigi::AnalysisResult result( path, 1, *m_writer, *m_streamAnalyzer );
        m_writer->addValue( &result, m_fieldRegister.pathField, path );
        string filename( "dummy.txt" );
        m_writer->addValue( &result, m_fieldRegister.filenameField, filename );
    }
    m_writer->commit();

    m_writer->deleteAllEntries();
    m_writer->commit();

    // now make sure nothing is left
    std::map<std::string, time_t> children;
    m_reader->getChildren( std::string(), children );
    CPPUNIT_ASSERT_MESSAGE( "Still files left after calling deleteAllEntries.", children.empty() );

    std::vector<IndexedDocument> results = m_reader->query( QueryParser::buildQuery( FieldRegister::pathFieldName + "=\"" + path + "\"" ), 0, 100 );
    CPPUNIT_ASSERT_MESSAGE( "Query results not empty after deleteAllEntries", results.empty() );

    results = m_reader->query( QueryParser::buildQuery( FieldRegister::filenameFieldName + "=\"dummy.txt\"" ), 0, 100 );
    CPPUNIT_ASSERT_MESSAGE( "Query results not empty after deleteAllEntries", results.empty() );
}


void IndexWriterTester::testDeleteEntries()
{
    // add some random data
    string path1( "/tmp/dummy1.txt" );
    string path2( "/tmp/dummy2.txt" );
    string filename1( "dummy1.txt" );
    string filename2( "dummy2.txt" );
    {
        AnalysisResult anaRes1( path1, 1, *m_writer, *m_streamAnalyzer );
        AnalysisResult anaRes2( path2, 1, *m_writer, *m_streamAnalyzer );

        m_writer->addValue( &anaRes1, m_fieldRegister.pathField, anaRes1.path() );
        m_writer->addValue( &anaRes1, m_fieldRegister.filenameField, filename1 );

        m_writer->addValue( &anaRes2, m_fieldRegister.pathField, anaRes2.path() );
        m_writer->addValue( &anaRes2, m_fieldRegister.filenameField, filename2 );
    }
    m_writer->commit();

    // now delete the first one
    std::vector<std::string> entries;
    entries.push_back( path1 );
    m_writer->deleteEntries( entries );
    m_writer->commit();

    // now make sure only the second one it left
    std::map<std::string, time_t> children;
    m_reader->getChildren( std::string(), children );
    CPPUNIT_ASSERT_EQUAL( 1, ( int )children.size() );
    CPPUNIT_ASSERT_EQUAL( path2, children.begin()->first );
}
