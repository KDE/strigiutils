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

#include <strigi/analysisresult.h>
#include <strigi/indexwriter.h>
#include <strigi/indexreader.h>
#include <strigi/fieldtypes.h>
#include <strigi/query.h>
#include <strigi/queryparser.h>

#include <sstream>

using namespace std;
using namespace strigiunittest;
using namespace Strigi;

void
IndexWriterTest::setUp() {
    IndexTest::setUp();
    m_streamAnalyzer = new StreamAnalyzer( m_analyzerConfiguration );
}

void
IndexWriterTest::tearDown() {
    delete m_streamAnalyzer;
    IndexTest::tearDown();
}

void
IndexWriterTest::testAddText() {
    // make sure the index is empty
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );

    // test adding very simple text
    string path( "/tmp/dummy.txt" );
    {
        Strigi::AnalysisResult result( path, 1, *m_writer, *m_streamAnalyzer );
        std::string text( "Some dummy text for testing." );
        m_writer->addText( &result, text.c_str(), (int32_t)text.length() );
    }
    m_writer->commit();
    // at this point the new document is most likely not yet visible
    // for query() calls, the reader is not forcibly refreshed
    // it *is* refreshed for calls to countDocuments(), so we call that
    CPPUNIT_ASSERT_EQUAL( 1, m_reader->countDocuments() );

    std::vector<IndexedDocument> results = m_reader->query(
        QueryParser::buildQuery("dummy"), 0, 100 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "got invalid number of results", 1,
        (int)results.size() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "got invalid path result", path,
        results[0].uri );

    m_writer->deleteAllEntries();

    // FIXME: test adding unicode text

}

void
IndexWriterTest::testDeleteAllEntries() {
    // make sure the index is empty
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );

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
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );

    // now make sure nothing is left
    std::map<std::string, time_t> children;
    m_reader->getChildren( std::string(), children );
    CPPUNIT_ASSERT_MESSAGE( "Still files left after calling deleteAllEntries.",
        children.empty() );

    std::vector<IndexedDocument> results = m_reader->query(
        QueryParser::buildQuery(
            FieldRegister::pathFieldName + "=\"" + path + "\"" ), 0, 100 );
    CPPUNIT_ASSERT_MESSAGE( "Query results not empty after deleteAllEntries",
       results.empty() );

    results = m_reader->query( QueryParser::buildQuery(
        FieldRegister::filenameFieldName + "=\"dummy.txt\"" ), 0, 100 );
    CPPUNIT_ASSERT_MESSAGE( "Query results not empty after deleteAllEntries",
        results.empty() );
}

void
IndexWriterTest::testDeleteEntries() {
    // make sure the index is empty
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );

    // add some random data
    string path1( "/tmp/dummy1.txt" );
    string path2( "/tmp/dummy2.txt" );
    string filename1( "dummy1.txt" );
    string filename2( "dummy2.txt" );
    {
        AnalysisResult anaRes1( path1, 1, *m_writer, *m_streamAnalyzer );
        AnalysisResult anaRes2( path2, 1, *m_writer, *m_streamAnalyzer );

        m_writer->addValue(&anaRes1, m_fieldRegister.pathField, anaRes1.path());
        m_writer->addValue(&anaRes1, m_fieldRegister.filenameField, filename1);

        m_writer->addValue(&anaRes2, m_fieldRegister.pathField, anaRes2.path());
        m_writer->addValue(&anaRes2, m_fieldRegister.filenameField, filename2 );
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
void
IndexWriterTest::testDeleteNestedEntries() {
    // make sure the index is empty
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );

    // add some random data
    string path1( "/tmp/dummydir" );
    string path2( "/tmp/dummydir/dummydir2" );
    string path3( "/tmp/dummydir/dummydir2/d.txt" );
    {
        AnalysisResult anaRes1(path1, 1, *m_writer, *m_streamAnalyzer);
        {
            AnalysisResult anaRes2(path2, 1, *m_writer, *m_streamAnalyzer,
                path1);
            {
                AnalysisResult anaRes3(path3, 1, *m_writer, *m_streamAnalyzer,
                    path2);
            }
        }
    }
    m_writer->commit();
    // check that three documents are in the index
    CPPUNIT_ASSERT_EQUAL(3, m_reader->countDocuments());

    m_writer->commit();
    // check that three documents are in the index, even after committing twice
    CPPUNIT_ASSERT_EQUAL(3, m_reader->countDocuments());

    // check that three documents are in the index
    CPPUNIT_ASSERT_EQUAL(3, m_reader->countDocuments());

    // now delete the first one
    std::vector<std::string> entries;
    entries.push_back(path1);
    m_writer->deleteEntries(entries);
    m_writer->commit();
    m_reader->countDocuments();

    // now make sure none are left 
    CPPUNIT_ASSERT_EQUAL( 0, m_reader->countDocuments() );
}
