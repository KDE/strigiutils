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

#ifndef _XESAM_UL_DRIVER
#define _XESAM_UL_DRIVER

#include "xesam_ul_parser.hh"

class XesamUlScanner;

namespace Strigi {
  class Query;
}

class XesamUlDriver
{
  public:
    XesamUlDriver();
    virtual ~XesamUlDriver();

    bool parseFile (const std::string &);
    bool parseString (const std::string &);
    XesamUlScanner* scanner() { return m_scanner; };

    Strigi::Query* query() { return m_query; };
    void setQuery (Strigi::Query* query) {m_query = query; };
    void addQuery (Strigi::Query*);
    void inspectQuery (Strigi::Query*);

    void setModifiers (std::string& modifiers) {m_modifiers = modifiers; }
    std::string& modifiers () { return m_modifiers; }
    void resetModifiers() { m_modifiers.clear();}
    void applyModifiers (Strigi::Query*, std::string&);

    void setNegate(bool negate) { m_negate = negate; }
    bool negate() { return m_negate; }

    void setError() { m_error = true; }
    
  private:
    XesamUlScanner* m_scanner;
    Strigi::Query* m_query;
    std::string m_modifiers;
    bool m_negate;
    bool m_error;

    /*
     * Prepend the relevant namespaces to the field names.
     * The rules for doing this are not quite clear at the moment. We always
     * prepend the xesam namespace for now.
     */
    void addNamespaces();
};

#endif
