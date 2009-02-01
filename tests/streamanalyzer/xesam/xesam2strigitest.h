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
#ifndef UNIT_TEST_XESAM2STRIGI_TEST_H
#define UNIT_TEST_XESAM2STRIGI_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Xesam2Strigi;

#include <set>
#include <string>

namespace strigiunittest
{
  class Xesam2StrigiTest : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( Xesam2StrigiTest );
    CPPUNIT_TEST( parseUlFromString );
    CPPUNIT_TEST( parseUlFromFile );
    CPPUNIT_TEST_SUITE_END();

    protected:
        Xesam2Strigi* xesam2strigi;
        std::set<std::string> m_ulQueries;

    public:
        void setUp();
        void tearDown();

        void parseUlFromString();
        void parseUlFromFile();
  };
}

#endif
