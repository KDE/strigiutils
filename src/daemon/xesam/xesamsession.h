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
#ifndef XESAMSESSION_H
#define XESAMSESSION_H

#include <string>
#include <list>
#include "variant.h"

class XesamLiveSearch;
class XesamSearch;
class XesamSession {
private:
    std::list<const XesamSearch*> searches;
    XesamLiveSearch& xesam;
    bool searchLive;
    bool searchBlocking;
    std::vector<std::string> hitFields;
    std::vector<std::string> hitFieldsExtended;
    int32_t hitSnippetLength;
    std::string sortPrimary;
    std::string sortSecondary;
    bool sortAscending;
public:
    XesamSession(XesamLiveSearch& x);
    ~XesamSession();
    Variant setProperty(const std::string& prop, const Variant& v);
    Variant getProperty(const std::string& prop);
    const std::string& newSearch(const std::string& xml_query);
    void closeSearch(const XesamSearch* search);
};

#endif
