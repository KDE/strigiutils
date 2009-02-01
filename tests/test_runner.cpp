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

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <stdexcept>

#include "strigilogging.h"
#include "config.h"

#include <iostream>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

int main() {
    // set some environment variables so that the system can find the desired
    // files
    setenv("XDG_DATA_HOME",
        SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("XDG_DATA_DIRS",
        SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("STRIGI_PLUGIN_PATH",
        BINARYDIR"/src/luceneindexer/" PATH_SEPARATOR
        BINARYDIR"/src/estraierindexer" PATH_SEPARATOR
        BINARYDIR"/src/sqliteindexer", 1);

    cerr << BINARYDIR << endl;

    STRIGI_LOG_INIT_BASIC()

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

    try
    {
//         std::cout << "Running "  <<  testPath;
        runner.run( controller);

        std::cerr << std::endl;

     // Print test in a compiler compatible format.
        CppUnit::CompilerOutputter outputter( &result, std::cerr );
        outputter.write();
    }
    catch ( std::invalid_argument &e )  // Test path not resolved
    {
        std::cerr  <<  std::endl
                <<  "ERROR: "  <<  e.what()
                << std::endl;
        return 0;
    }

    return result.wasSuccessful() ? 0 : 1;
}

