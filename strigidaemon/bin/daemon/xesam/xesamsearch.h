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
#ifndef STRIGI_XESAMSEARCH_H
#define STRIGI_XESAMSEARCH_H

#include <string>
#include <vector>
#include <list>
#include <strigi/query.h>
#include "strigi/strigi_thread.h"

namespace Strigi {
class Variant;
}
class XesamSession;
class XesamSearch {
private:
    class Private;
    Private* p;

    XesamSearch();
public:
    XesamSearch(XesamSession& s, const std::string& n,
        const std::string& query);
    XesamSearch(const XesamSearch&);
    XesamSearch(Private* p);
    ~XesamSearch();
    void operator=(const XesamSearch& xs);
    bool operator==(const XesamSearch& xs) { return p == xs.p; }
    void startSearch();
    void getHitCount(void* msg);
    void getHits(void* msg, uint32_t num);
    void getHitData(void* msg,
        const std::vector<uint32_t>& hit_ids,
        const std::vector<std::string>& properties);

    XesamSession session() const;
    std::string name() const;
    Strigi::Query query() const;
    // once the count has been determined, send out the messages
    void setCount(int c);
    bool isValid() const;
};

#endif
