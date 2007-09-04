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

#include "xesam2strigi.h"
#include "query.h"
#include "xesam_ul_driver.hh"
#include "XesamQLParser.h"
#include "StrigiQueryBuilder.h"


using std::string;
using namespace Dijon;

Xesam2Strigi::Xesam2Strigi ()
{
  m_query = 0;
}

Xesam2Strigi::~Xesam2Strigi ()
{
  if (m_query) {
    delete m_query;
    m_query = 0;
  }
}

bool Xesam2Strigi::parse (const string& xesam_query, Type query_type)
{
  if (query_type == QueryLanguage) {
    XesamQLParser xesamQlParser;
    StrigiQueryBuilder strigiQueryBuilder;

    if (m_query) {
      delete m_query;
      m_query = 0;
    }
    
    if (!xesamQlParser.parse ( xesam_query, strigiQueryBuilder))
      return false;

    m_query = new Strigi::Query (strigiQueryBuilder.get_query());
    return true;
  }
  else if (query_type == UserLanguage) {
    XesamUlDriver driver;
    
    if (!driver.parseString (xesam_query))
      return false;

    m_query = new Strigi::Query (*driver.query());
    return true;
  }

  // it won't happen
  return false;
}

bool Xesam2Strigi::parse_file (const string& xesam_query_file, Type query_type)
{
  if (m_query) {
    delete m_query;
    m_query = 0;
  }
  
  if (query_type == QueryLanguage) {
    XesamQLParser xesamQlParser;
    StrigiQueryBuilder strigiQueryBuilder;

    if (!xesamQlParser.parse_file ( xesam_query_file, strigiQueryBuilder))
      return false;

    m_query = new Strigi::Query (strigiQueryBuilder.get_query());
    return true;
  }
  else if (query_type == UserLanguage) {
    XesamUlDriver driver;
    
    if (!driver.parseFile (xesam_query_file))
      return false;

    m_query = new Strigi::Query (*driver.query());
    return true;
  }

  // it won't happen
  return false;
}

Strigi::Query Xesam2Strigi::query()
{
  if (m_query)
    return Strigi::Query(*m_query);
  else
    return Strigi::Query();
}
