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

#include "xesam_ul_driver.hh"
#include "xesam_ul_parser.hh"
#include "xesam_ul_file_scanner.h"
#include "xesam_ul_string_scanner.h"
#include "query.h"

#include <fstream>
#include <string>

using namespace std;

XesamUlDriver::XesamUlDriver()
{
  m_scanner = 0;
  m_query = 0;
  m_negate = false;
  m_error = false;
}

XesamUlDriver::~XesamUlDriver()
{
  if (m_scanner)
    delete m_scanner;

  if (m_query)
    delete m_query;
}

bool XesamUlDriver::parseFile (const std::string &filename)
{
  if (m_scanner)
    delete m_scanner;

  m_scanner = new XesamUlFileScanner (filename);
  yy::xesam_ul_parser parser (this);
  parser.parse ();

  delete m_scanner;
  m_scanner = 0;

  return m_error;
}

bool XesamUlDriver::parseString (const std::string &query)
{
  if (m_scanner)
    delete m_scanner;

  m_scanner = new XesamUlStringScanner (query);
  yy::xesam_ul_parser parser (this);
  parser.parse ();

  delete m_scanner;
  m_scanner = 0;

  return m_error;
}

void XesamUlDriver::addQuery(Strigi::Query* query)
{
  if (m_query) {
    m_query->subQueries().push_back(*query);
    delete query;
  }
  else
    m_query = query;
}

void XesamUlDriver::inspectQuery (Strigi::Query* query)
{
  cout << *query;
}

void XesamUlDriver::applyModifiers (Strigi::Query* query, string& modifiers)
{
  // looking at modifiers values we don't have to care about utf8 values
  // --> std::string::length() will return the exact string length
  for (unsigned int i = 0; i < modifiers.length(); i++) {
    switch (modifiers[i])
    {
      case 'b': /*Boost*/
        query->setBoost(2);
        break;
      case 'c': /*Case sensitive*/
        query->term().setCaseSensitive(true);
        break;
      case 'C': /*Case insensitive*/
        query->term().setCaseSensitive(false);
        break;
      case 'd': /*Diacritic sensitive*/
        query->term().setDiacriticSensitive(true);
        break;
      case 'D': /*Diacritic insensitive*/
        query->term().setDiacriticSensitive(false);
        break;
      case 'e': /*Exact match. Short for cdl*/
        query->term().setCaseSensitive(true);
        query->term().setDiacriticSensitive(true);
        query->term().setStemming(false);
        break;
      case 'f': /*Fuzzy search*/
        query->term().setFuzzy(0.5);
        break;
      case 'l': /*Don't do stemming*/
        query->term().setStemming(false);
        break;
      case 'L': /*Do stemming*/
        query->term().setStemming(true);
        break;
      case 'o': /*Ordered words*/
        query->term().setOrdered(true);
        break;
      case 'p': /*Proximity search (suggested default: 10)*/
        query->setType(Strigi::Query::Proximity);
        query->term().setProximityDistance(10);
        break;
      case 'r': /*The phrase is a regular expression*/
        query->setType(Strigi::Query::RegExp);
        break;
      case 's': /*Sloppy search*/
        query->term().setSlack(1);
        break;
      case 'w': /*Word based matching*/
        //TODO
        break;
    }
  }
}
