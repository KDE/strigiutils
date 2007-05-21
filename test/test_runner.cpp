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

#include <TestCaller.h>
#include <extensions/TestFactoryRegistry.h>
#include <TestResult.h>
#include <TextTestRunner.h>

int main()
{
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextTestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
//     runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
//                          std::cerr ) );
  // Run the tests.
    bool wasSucessful = runner.run();

  // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}

// using namespace CppUnit;
// 
// int main (int argc, char* argv[]) {
//     TextTestRunner runner;
//     TestFactoryRegistry& registry = TestFactoryRegistry::getRegistry();
// 
//      // run all tests if none specified on command line
//     Test* test_to_run = registry.makeTest();
//     if (argc>1)
//         test_to_run = test_to_run->findTest(argv[1]);
// 
//     runner.addTest( test_to_run );
//     bool failed = runner.run("", false);
//     return !failed;
// }
