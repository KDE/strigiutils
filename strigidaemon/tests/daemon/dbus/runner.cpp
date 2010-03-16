/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

#include "strigidaemonunittestsession.h"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/TestResultCollector.h>

#include <stdexcept>

#include "config.h"

using namespace std;

int
doTests() {
    // Get the top level suite from the registry
    CppUnit::Test *suite;
    suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextTestRunner runner;
    runner.addTest( suite );

    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener( &result );

    // Add a listener that print dots as test run.
    CppUnit::TextTestProgressListener progress;
    controller.addListener( &progress );

    try {
        runner.run( controller);

        std::cerr << std::endl;

     // Print test in a compiler compatible format.
        CppUnit::CompilerOutputter outputter( &result, std::cerr );
        outputter.write();
    } catch ( std::invalid_argument &e )  // Test path not resolved
    {
        std::cerr  <<  std::endl
                <<  "ERROR: "  <<  e.what()
                << std::endl;
        return 0;
    }
    return result.wasSuccessful() ? 0 : 1;
}
int
main(int argc, char** argv) {
    StrigiDaemonUnitTestSession session;
    session.setStrigiDaemon(BINARYDIR"/src/daemon/strigidaemon");
    session.setXDGDir(SOURCEDIR"/src/streamanalyzer/fieldproperties");
    session.addBackend("clucene", BINARYDIR"/src/luceneindexer");
    session.setStrigiDir("strigitestdir");
    session.setDataDir("testdatadir");
    session.addFile("test", "hello\n");
    session.addFile("test too", "hello world\n");
    session.addFile(".hiddenfile", "hello this is hidden\n");
    session.addFile(".hiddendir/file", "hello this is hidden\n");
    session.addFile("normaldir/.hidden/f", "hello this should be hidden\n");
    session.start();

    int result = doTests();

    session.stop();
    return result;
}

