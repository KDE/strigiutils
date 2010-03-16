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
#include <strigi/variant.h>

class XesamLiveSearch;
class XesamSearch;
class XesamSession {
private:
    class Private;
    Private* p;

    XesamSession();
public:
    XesamSession(XesamLiveSearch& x);
    XesamSession(const XesamSession& xs);
    ~XesamSession();
    void operator=(const XesamSession& xs);
    bool operator==(const XesamSession& xs) { return p == xs.p; }
    /**
     * Attempt to set a parameter to the given value.
     * A runtime_error will be thrown if this function was called after a
     * search has been created.
     **/
    Strigi::Variant setProperty(const std::string& prop, const Strigi::Variant& v);
    Strigi::Variant getProperty(const std::string& prop);
    std::string newSearch(const std::string& xml_query);
    void closeSearch(const XesamSearch& search);

    XesamLiveSearch& liveSearch() const;
    std::vector<std::string>& hitFields() const;
};

#endif
