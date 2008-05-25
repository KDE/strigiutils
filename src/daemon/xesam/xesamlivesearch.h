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
#ifndef STRIGI_XESAMLIVESEARCH_H
#define STRIGI_XESAMLIVESEARCH_H

#include "xesamlivesearchinterface.h"

/**
 * The complete spec implemented by this interface is here:
 * http://xesam.org/main/XesamSearch90
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
    XesamLiveSearch(const XesamLiveSearch&);
    void operator=(const XesamLiveSearch&);
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
    Strigi::Variant SetProperty(const std::string& session, const std::string& prop,
        const Strigi::Variant& v);
    /**
     * Get the value of a property.
     **/
    Strigi::Variant GetProperty(const std::string& session, const std::string& prop);
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
    void GetHitCount(void* msg, const std::string& search);
    /**
     * This function is called by the implementation to send the hit count.
     **/
    void GetHitCountResponse(void* msg, const char* err, uint32_t count);
    /**
     * Return num hits. If search.blocking==true this call blocks until there is
     * num hits available or the index has been fully searched. The client
     * should keep track of each hit's serial number if it want to use
     * GetHitData later. See below for a discussion about the return value.
     **/
    void GetHits(void* msg, const std::string& search, uint32_t num);
    /**
     * This function is called by the implementation to sent the hit results.
     **/
    void GetHitsResponse(void* msg, const char* err,
            const std::vector<std::vector<Strigi::Variant> >& h);
    /**
     * Get hit metadata. Intended for snippets or modified hits. hit_ids are
     * serial numbers as obtained from GetHits. The requested properties does
     * not have to be the ones listed in in the hit.fields session property.
     **/
    void GetHitData(void* msg, const std::string& search,
            const std::vector<uint32_t>& hit_ids,
            const std::vector<std::string>& properties);
    /**
     * This function is called by the implementation to sent the hits data.
     **/
    void GetHitDataResponse(void* msg, const char* err,
        const std::vector<std::vector<Strigi::Variant> >& d);
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
     * Functions called by the implementation to signal events.
     */
    /**
     * Signal that new hits have been found.
     * @param count the number of hits added
     **/
    void HitsAdded(const std::string& search, const uint32_t count);
    /**
     * Signal that hits have been removed because the corresponding files
     * no longer match or have been removed.
     **/
    void HitsRemoved(const std::string& search,
        const std::vector<uint32_t>& hit_ids);
    /**
     * Signal that a document that was being watched has been modified.
     * The document still matches the hit.
     **/
    void HitsModified(const std::string& search,
        const std::vector<uint32_t>& hit_ids);
    /**
     * The given search has scanned the entire index. For non-live searches this means that
     * no more hits will be available. For a live search this means that all future signals 
     * (Hits{Added,Removed,Modified}) will be related to objects that changed in the index.
     **/
    void SearchDone(const std::string& search);
    /**
     * When the state as returned by GetState changes the StateChanged signal is fired with
     * an argument as described in said method. If the indexer expects to only enter the
     * UPDATE state for a very brief period - fx. indexing one changed file - it is not
     * required that the StateChanged signal is fired. The signal only needs to be fired if
     * the process of updating the index is going to be non-negligible. The purpose of this
     * signal is not to provide exact details on the engine, just to provide hints for a
     * user interface.
     **/
    void StateChanged(const std::vector<std::string>& state_info);

    // access to IndexManager for searches
    Strigi::IndexManager* indexManager() const;

    // internal
    void addSearch(const std::string&, XesamSearch&);
    void removeSearch(const std::string&);
    JobQueue& queue() const;
};

#endif
