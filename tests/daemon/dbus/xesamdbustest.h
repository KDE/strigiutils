/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2008 Jos van den Oever <jos@vandenoever.info>
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
#ifndef UNIT_TEST_XESAMDBUSTEST_H
#define UNIT_TEST_XESAMDBUSTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class OrgFreedesktopXesamSearchInterface;
class XesamListener;

namespace strigiunittest {

class XesamDBusTest : public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE( XesamDBusTest );
    CPPUNIT_TEST( testSimpleSession );
    CPPUNIT_TEST( testSimpleSearch );
    CPPUNIT_TEST( testSimpleSearchSignals );
    CPPUNIT_TEST( testGetProperty );
    CPPUNIT_TEST( testSetProperty );
    CPPUNIT_TEST( testTwoTermSearch );
    CPPUNIT_TEST( testHitFields );
    CPPUNIT_TEST_SUITE_END();
private:
    OrgFreedesktopXesamSearchInterface* xesam;
    XesamListener* listener;
public:

    void setUp();
    void tearDown();

    void testSimpleSession();
    void testSimpleSearch();
    void testSimpleSearchSignals();
    void testGetProperty();
    void testSetProperty();
    void testTwoTermSearch();
    void testHitFields();
    bool waitForStatusIdle(int milliseconds);
};

}

#endif
