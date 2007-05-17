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
#ifndef XESAMLIVESEARCHINTERFACE_H
#define XESAMLIVESEARCHINTERFACE_H

#include <string>
#include <vector>
#include <map>

/**
 * Simple inefficient implementation of a variant type as needed by xesam
 **/
class Variant {
private:
    bool b_value;
    int32_t i_value;
    std::string s_value;
    std::vector<std::string> ss_value;
public:
};

class XesamLiveSearchInterface {
public:
    XesamLiveSearchInterface() {}
    std::string NewSession();
    Variant SetProperty(const std::string& session, const std::string& prop,
        const Variant& v);
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

    virtual void HitsAdded(const std::string& search, const int32_t count) = 0; 
    virtual void HitsRemoved(const std::string& search,
        const std::vector<int32_t>& hit_ids) = 0;
    virtual void HitsModified(const std::string& search,
        const std::vector<int32_t>& hit_ids) = 0;
};

#endif
