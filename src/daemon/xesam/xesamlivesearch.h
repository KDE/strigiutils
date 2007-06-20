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

/**
 * The complete spec implemented by this interface is here:
 * http://www.freedesktop.org/wiki/XesamSearchLive
 **/

namespace Strigi {
    class IndexManager;
}

class JobQueue;
class XesamSearch;
class XesamLiveSearch : public XesamLiveSearchInterface {
private:
    std::vector<XesamLiveSearchInterface*> ifaces;
    class Private;
    Private* const p;
public:
    XesamLiveSearch(Strigi::IndexManager*, JobQueue&);
    ~XesamLiveSearch();

    void addInterface(XesamLiveSearchInterface* i) {
        ifaces.push_back(i);
    }

    /**
     * Start a new session, return the name of the session.
     **/
    std::string NewSession();
    /**
     * Change the value of a session property, return new value of the propery
     * or the old value if it did not chanege.
     **/
    Variant SetProperty(const std::string& session, const std::string& prop,
        const Variant& v);
    /**
     * Get the value of a property.
     **/
    Variant GetProperty(const std::string& session, const std::string& prop);
    /**
     * Close a session and all associated searches.
     **/
    void CloseSession(const std::string& session);
    /**
     * Build a new search from a query and return the name of the new search.
     **/
    std::string NewSearch(const std::string& session,
            const std::string& query_xml);
    /**
     * Start the new search. The client confirms that it has received the handle
     * to the search. From this point on the server can start emitting signals
     * from this search.
     **/
    void StartSearch(const std::string& search);
    /**
     * Return the number of hits found so far for this search.
     * If search.blocking==true this call blocks until the index has been
     * fully searched.
     **/
    void CountHits(void* msg, const std::string& search);
    /**
     * This function is called by the implementation to send the hit count.
     **/
    void CountHitsResponse(void* msg, int32_t count);
    /**
     * Return num hits. If search.blocking==true this call blocks until there is
     * num hits available or the index has been fully searched. The client
     * should keep track of each hit's serial number if it want to use
     * GetHitData later. See below for a discussion about the return value.
     **/
    std::vector<std::vector<Variant> > GetHits(const std::string& search,
            int32_t num);
    /**
     * Get hit metadata. Intended for snippets or modified hits. hit_ids are
     * serial numbers as obtained from GetHits. The requested properties does
     * not have to be the ones listed in in the hit.fields session property.
     **/
    std::vector<std::vector<Variant> > GetHitData(const std::string& search,
            const std::vector<int32_t>& hit_ids,
            const std::vector<std::string>& properties);
    /**
     * Close and free a search. Closing your session also closes all searches in
     * that session.
     **/
    void CloseSearch(const std::string& search);
    /**
     * Get information about the status of the search engine. state_info is an
     * array of two strings. The value at position zero is one of IDLE, UPDATE,
     * or FULL_INDEX. The value at position one is a string formatted integer in
     * the range 0-100. In the case of IDLE the value should be ignored,
     * otherwise it represents the number of percent the task is done. 
     * - IDLE,       the search engine is not doing anything (other than maybe
     *               handling other searches) 
     * - UPDATE,     the index is being updated 
     * - FULL_INDEX, a new index is being build from scratch
     **/
    std::vector<std::string> GetState();
    /**
     * SIGNALS
     * Functionscalled by the implementation to signal events.
     */
    /**
     * Signal that new hits have been found.
     * @param count the number of hits added
     **/
    void HitsAdded(const std::string& search, const int32_t count);
    /**
     * Signal that hits have been removed because the corresponding files
     * no longer match or have been removed.
     **/
    void HitsRemoved(const std::string& search,
        const std::vector<int32_t>& hit_ids);
    /**
     * Signal that a document that was being watched has been modified.
     * The document still matches the hit.
     **/
    void HitsModified(const std::string& search,
        const std::vector<int32_t>& hit_ids);

    // access to IndexManager for searches
    Strigi::IndexManager* indexManager() const;

    // internal
    void addSearch(const std::string&, XesamSearch*);
    void removeSearch(const std::string&);
    JobQueue& queue() const;
};

#endif
