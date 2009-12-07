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

#include "xesam_ul_file_scanner.h"

using namespace std;

XesamUlFileScanner::XesamUlFileScanner(string filename)
{
  m_ifstream.open(filename.c_str());
}

XesamUlFileScanner::~XesamUlFileScanner()
{
  if (m_ifstream.is_open())
    m_ifstream.close();
}

bool XesamUlFileScanner::is_open()
{
  return m_ifstream.is_open();
}

char XesamUlFileScanner::getCh()
{
  char ret;
  m_ifstream.get(ret);
  return ret;
}

char XesamUlFileScanner::peekCh()
{
  return (char)m_ifstream.peek();
}

bool XesamUlFileScanner::eof()
{
  return m_ifstream.eof();
}
