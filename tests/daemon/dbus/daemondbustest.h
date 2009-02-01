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
#ifndef UNIT_TEST_DAEMONDBUSTEST_H
#define UNIT_TEST_DAEMONDBUSTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <string>
#include "strigiclient.h"

namespace strigiunittest {

class DaemonDBusTest : public CppUnit::TestFixture {
private:

    CPPUNIT_TEST_SUITE( DaemonDBusTest );
    CPPUNIT_TEST( testProcess );
    CPPUNIT_TEST( testStatusOfEmptyIndex );
    CPPUNIT_TEST( testStopDaemon );
    CPPUNIT_TEST( testIndexing );
    CPPUNIT_TEST( testSimpleQuery );
    CPPUNIT_TEST_SUITE_END();
private:
    StrigiClient strigiclient;
    
    /* Wait until the strigidaemon returns to the status 'idling'.
       This function is useful when waiting for strigidaemon to start up or for waiting for it
       to finish indexing.
       @parem milliseconds the maximal time to wait in milliseconds
       @return true is the daemon reached the status 'idling'
     */
    bool waitForStatusIdle(int milliseconds);
public:
    void setUp();
    void tearDown();

    void testProcess();
    void testStatusOfEmptyIndex();
    void testStopDaemon();
    void testIndexing();
    void testSimpleQuery();
};

}

#endif
