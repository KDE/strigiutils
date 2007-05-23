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
#ifndef XESAMLIVESEARCH_H
#define XESAMLIVESEARCH_H

#include "xesamlivesearchinterface.h"

namespace Strigi {
    class IndexManager;
}

class XesamSearch;
class XesamLiveSearch : public XesamLiveSearchInterface {
private:
    std::vector<XesamLiveSearchInterface*> ifaces;
    class Private;
    Private* const p;
public:
    XesamLiveSearch(Strigi::IndexManager*);
    ~XesamLiveSearch();

    void addInterface(XesamLiveSearchInterface* i) {
        ifaces.push_back(i);
    }

    std::string NewSession();
    Variant SetProperty(const std::string& session, const std::string& prop,
        const Variant& v);
    Variant GetProperty(const std::string& session, const std::string& prop);
    void CloseSession(const std::string& session);
    std::string NewSearch(const std::string& session,
            const std::string& query_xml);
    int32_t CountHits(const std::string& search);
    std::vector<std::vector<Variant> > GetHits(const std::string& search,
            int32_t num);
    std::vector<std::vector<Variant> > GetHitData(const std::string& search,
            const std::vector<int32_t>& hit_ids,
            const std::vector<std::string>& properties);
    void CloseSearch(const std::string& search);
    std::vector<std::string> GetState();

    void HitsAdded(const std::string& search, const int32_t count); 
    void HitsRemoved(const std::string& search,
        const std::vector<int32_t>& hit_ids);
    void HitsModified(const std::string& search,
        const std::vector<int32_t>& hit_ids);

    // access to IndexManager for searches
    Strigi::IndexManager* indexManager() const;

    // internal
    void addSearch(const std::string&, XesamSearch*);
    void removeSearch(const std::string&);
};

#endif
