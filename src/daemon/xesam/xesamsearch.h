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
#ifndef XESAMSEARCH_H
#define XESAMSEARCH_H

#include <string>
#include <vector>

class Variant;
class XesamSession;
class XesamSearch {
public:
    const std::string name;
    XesamSession& session;

    XesamSearch(XesamSession& s, const std::string& n,
        const std::string& query) :name(n), session(s) {}
    ~XesamSearch() { }
    int32_t countHits() { return 10; }
    std::vector<std::vector<Variant> > getHits(int32_t num);
    std::vector<std::vector<Variant> > getHitData(
        const std::vector<int32_t>& hit_ids,
        const std::vector<std::string>& properties);
};

#endif
