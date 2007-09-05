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

#include "xesam2strigitest.h"
#include "xesam2strigi.h"

using namespace std;
using namespace strigiunittest;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( Xesam2StrigiTest );

void Xesam2StrigiTest::setUp() {
  //africa.txt
  m_ulQueries.insert ("type:image size>=1mb tag:flower africa");
  //africa2.txt
  m_ulQueries.insert ("type:image size>=1mb tag:flower south africa");
  //africa2.txt
  m_ulQueries.insert ("type:image size>=1mb tag:flower south africa");
  //helloworld.txt
  m_ulQueries.insert ("\"hello world, how are you?\"cle");
  //helloworld2.txt
  m_ulQueries.insert ("\"hello world, how are you?\"CL fine thanks");
  //helloworld3.txt
  m_ulQueries.insert ("+ \"hello world, how are you?\"cS and fine thanks");
  //hendrix.txt
  m_ulQueries.insert ("type:music hendrix");
  //hendrix2.txt
  m_ulQueries.insert ("type:music creator=\"James Marshall Hendrix\"");
  //negate.txt
  m_ulQueries.insert ("-type:music hendrix");

  xesam2strigi = new Xesam2Strigi();
}

void Xesam2StrigiTest::tearDown()
{
  if (xesam2strigi)
    delete xesam2strigi;
  xesam2strigi = NULL;
}

void Xesam2StrigiTest::parseUlFromString()
{
  for (set<string>::iterator iter = m_ulQueries.begin();
       iter != m_ulQueries.end(); iter++)
  {
    if (!xesam2strigi->parse (*iter, Xesam2Strigi::UserLanguage)) {
      string msg = "Failed to parse xesam userlanguage query: |";
      msg += *iter;
      msg += "| from string";
      CPPUNIT_FAIL (msg);
    }
  }
}
