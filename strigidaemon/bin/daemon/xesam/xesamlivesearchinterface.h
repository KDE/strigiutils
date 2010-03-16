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

#include <strigi/variant.h>
#include <map>

class XesamLiveSearchInterface {
private:
    XesamLiveSearchInterface* const iface;
    XesamLiveSearchInterface(const XesamLiveSearchInterface&);
    void operator=(const XesamLiveSearchInterface&);
public:
    XesamLiveSearchInterface(XesamLiveSearchInterface* x) :iface(x) {}
    virtual ~XesamLiveSearchInterface() {}
    virtual std::string /*session*/ NewSession() {
        return iface->NewSession();
    }
    virtual Strigi::Variant /*new_val*/ SetProperty(const std::string& session,
            const std::string& prop, const Strigi::Variant& val) {
        return iface->SetProperty(session, prop, val);
    }
    virtual Strigi::Variant /*value*/ GetProperty(const std::string& session,
            const std::string& prop) {
        return iface->GetProperty(session, prop);
    }
    virtual void CloseSession(const std::string& session) {
        iface->CloseSession(session);
    }
    virtual std::string /*search*/ NewSearch(const std::string& session,
            const std::string& query_xml) {
        return iface->NewSearch(session, query_xml);
    }
    virtual void StartSearch(const std::string& search) {
        iface->StartSearch(search);
    }
    virtual void GetHitCount(void* msg, const std::string& search) {
        iface->GetHitCount(msg, search);
    }
    virtual void GetHitCountResponse(void* msg, const char* err, uint32_t count) = 0;
    virtual void GetHits(void* msg, const std::string& search, uint32_t num) {
        iface->GetHits(msg, search, num);
    }
    virtual void GetHitsResponse(void* msg, const char* err,
            const std::vector<std::vector<Strigi::Variant> >& hits) = 0;
    virtual void GetHitData(void* msg,
            const std::string& search,
            const std::vector<uint32_t>& hit_ids,
            const std::vector<std::string>& fields) {
        iface->GetHitData(msg, search, hit_ids, fields);
    }
    virtual void GetHitDataResponse(void* msg, const char* err,
            const std::vector<std::vector<Strigi::Variant> >& hit_data) = 0;
    virtual void CloseSearch(const std::string& search) {
        return iface->CloseSearch(search);
    }
    virtual std::vector<std::string> /*state_info*/ GetState() {
        return iface->GetState();
    }

    virtual void HitsAdded(const std::string& search, const uint32_t count) = 0; 
    virtual void HitsRemoved(const std::string& search,
        const std::vector<uint32_t>& hit_ids) = 0;
    virtual void HitsModified(const std::string& search,
        const std::vector<uint32_t>& hit_ids) = 0;
    virtual void SearchDone(const std::string& search) = 0;
    virtual void StateChanged(const std::vector<std::string>& state_info) = 0;
};

#endif
