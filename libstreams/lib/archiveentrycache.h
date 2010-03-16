/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006,2009 Jos van den Oever <jos@vandenoever.info>
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
#ifndef ARCHIVEENTRYCACHE_H
#define ARCHIVEENTRYCACHE_H

#include <strigi/substreamprovider.h>

/**
 * @brief A cache for entries that have already been found.
 * @internal
 *
 * The cache is stored in a tree, sorted on the path in the URL
 * (with @c / as the delimiter).
 */
class ArchiveEntryCache {
public:
    class SubEntry;
    typedef std::map<std::string, SubEntry *> SubEntryMap;

    /**
     * @brief Represents an entry in the cache.
     */
    class SubEntry {
    public:
        /** The cached information about this entry. */
        Strigi::EntryInfo entry;
        /** A list of subentries of this entry. */
        SubEntryMap entries;
        /** Constructor */
        SubEntry() { }
        /** Destructor, recursively frees its entries. */
        virtual ~SubEntry();
    };
    /**
     * @brief Represents a top-level entry in the cache
     */
    class RootSubEntry : public SubEntry {
    public:
        /** Constructor */
        RootSubEntry() :SubEntry() {}
        /** Whether this entry and its subentries have been indexed yet */
        bool indexed;
        const SubEntry* findEntry(const std::string& rootpath,
            const std::string& url) const;
    };
    ~ArchiveEntryCache() {
       std::map<std::string, RootSubEntry*>::const_iterator i;
       for (i = cache.begin(); i != cache.end(); ++i) {
           delete i->second;
       }
    }
    /**
     * @brief The cache itself.
     *
     * Contains all the root entries in the cache.
     */
    typedef std::map<std::string, RootSubEntry*> RootCache;
    RootCache cache;
    /**
     * @brief Find an entry in the cache by its URL
     *
     * @param url the url of the entry
     * @return a pointer to the entry, if it is in the cache,
     * 0 if the entry is not in the cache
     */
    const SubEntry* findEntry(const std::string& url) const;
    /**
     * @brief Finds the root subentry in the cache of the given url,
     * if it is in the cache
     *
     * This finds the RootSubEntry (tree root) that the SubEntry for the
     * given url either is or should be stored under.  If there is no
     * suitable RootSubEntry in the cache, cache.end() is returned.
     *
     * @param url the url of the object to find the root entry for
     * @return an iterator to the root entry
     */
    std::map<std::string, RootSubEntry*>::const_iterator findRootEntry(
        const std::string& url) const;
};

#endif
