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
#include <strigi/archivereader.h>
#include <strigi/sdfinputstream.h>
#include <strigi/tarinputstream.h>
#include <strigi/mailinputstream.h>
#include <strigi/rpminputstream.h>
#include <strigi/arinputstream.h>
#include <strigi/zipinputstream.h>
#include "archiveentrycache.h"
#include "listinginprogress.h"
#include <iostream>
#include <set>

using namespace std;
using namespace Strigi;

class ArchiveReader::DirLister::Private {
private:
    int pos;
    vector<EntryInfo> entries;
    ListingInProgress* listing;
    const ArchiveEntryCache::SubEntry* entry;
    set<std::string> done;
public:
    const std::string url;
    explicit Private(const vector<EntryInfo>& v, int p = 0)
        :pos(p), entries(v), listing(NULL), entry(NULL) {
    }
    explicit Private(ListingInProgress* l, const std::string& u)
            :pos(0), listing(l), entry(NULL), url(u) {
        listing->ref();
    }
    ~Private() {
        if (listing && listing->unref()) delete listing;
    }
    void operator=(const Private& a) {
        pos = a.pos;
        entries = a.entries;
        if (listing && listing->unref()) delete listing;
        listing = a.listing;
        if (listing) listing->ref();
        entry = a.entry;
        done = a.done;
    }
    bool
    nextEntry(EntryInfo& e) {
        if (pos < 0 || (listing == NULL && pos >= (int)entries.size())) {
            // there are no (more) valid entries
            pos = -1;
        } else if (listing == NULL) {
            // get the next entry from the static list
            e = entries[pos++];
        } else if (entry == NULL) {
            // acquire a dynamic list and get the first entry
            entry = listing->nextEntry(url);
            if (entry && entry->entries.size()) {
                e = nextEntry();
                pos = 1;
            } else {
                pos = -1;
            }
        } else if (pos < (int)entry->entries.size()
                || listing->nextEntry(entry)) {
            // e is now the next entry from the dynamic list
            e = nextEntry();
        } else {
            // there are no more entries
            pos = -1;
        }
        return pos != -1;
    }
private:
    /**
     * Get the first entry from entry->entries() that is not in the entries
     * vector.
     **/
    EntryInfo
    nextEntry() {
        // we can make this more efficient by sorting a set with the entries
        // that have already been returned. A set is faster than a vector for
        // lookups by value.
        ArchiveEntryCache::SubEntryMap::const_iterator i
            = entry->entries.begin();
        do {
            if (done.find(i->first) == done.end()) {
                done.insert(i->first);
                pos++;
                return i->second->entry;
            }
        } while (++i != entry->entries.end());
        // this should never happen!
        return i->second->entry;
    }
};

ArchiveReader::DirLister::DirLister(Private* d) :p(d) {
    assert(d);
}

ArchiveReader::DirLister::DirLister(const DirLister& dl)
    :p(new Private(*dl.p)) {
}

ArchiveReader::DirLister::~DirLister() {
    delete p;
}

bool
ArchiveReader::DirLister::nextEntry(EntryInfo& e) {
    return p->nextEntry(e);
}
const ArchiveReader::DirLister&
ArchiveReader::DirLister::operator=(const DirLister& a) {
    *p = *a.p;
    return a;
}

/** Private data members and functions for ArchiveReader */
class ArchiveReader::ArchiveReaderPrivate {
public:
    /** List of streams that have been opened for openStream() calls. **/
    typedef std::map<InputStream*, std::list<StreamPtr> > OpenstreamsType;
    OpenstreamsType openstreams;
    /** List of objects that can open streams. **/
    std::list<StreamOpener*> openers;
    /** Cache of EntryInfo object to files that have been read. **/
    ArchiveEntryCache cache;
    /** Map with pointers to a headerCheck function as key and a factory
     *  function as value. */
    std::map<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)> subs;
    /**
     * A list of dir listings in progress. Each ListingInProgress has a physical
     * file in common. The url of the physical file is the key.
     **/
    std::map<std::string, ListingInProgress*> listingsInProgress;

    std::vector<size_t> cullName(const std::string& url,
        InputStream*& stream) const;
    SubStreamProvider* positionedProvider(const std::string& url);
    InputStream* open(const std::string& url) const;
    /**
     * @brief Query known StreamOpeners about an URL.
     *
     * Adds the info to the cache if it was found
     *
     * @param url the URL to query
     * @param e the EntryInfo structure to populate with the information
     * @return 0 on success, -1 if the url was not found
     */
    int localStat(const std::string& url, EntryInfo& e);
    ArchiveReaderPrivate();
    ~ArchiveReaderPrivate();
    ListingInProgress* findListingInProgress(const string& url) const;
};
ArchiveReader::ArchiveReaderPrivate::ArchiveReaderPrivate() {
    typedef std::pair<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)> SubsPair;

    subs.insert(SubsPair(MailInputStream::checkHeader,
        MailInputStream::factory));
    subs.insert(SubsPair(RpmInputStream::checkHeader,
        RpmInputStream::factory));
    subs.insert(SubsPair(ArInputStream::checkHeader,
        ArInputStream::factory));
    subs.insert(SubsPair(ZipInputStream::checkHeader,
        ZipInputStream::factory));
    subs.insert(SubsPair(TarInputStream::checkHeader,
        TarInputStream::factory));
    subs.insert(SubsPair(SdfInputStream::checkHeader,
        SdfInputStream::factory));
}
ArchiveReader::ArchiveReaderPrivate::~ArchiveReaderPrivate() {
    if (openstreams.size() > 0) {
        cerr << openstreams.size() << " streams were not closed." << endl;
        OpenstreamsType::iterator i;
        for (i = openstreams.begin(); i != openstreams.end(); ++i) {
            free(i->second);
        }
    }
    map<string, ListingInProgress*>::const_iterator end
        = listingsInProgress.end();
    for (map<string, ListingInProgress*>::const_iterator i
            = listingsInProgress.begin(); i != end; ++i) {
        if (i->second->unref()) delete i->second;
    }
}
/**
 * Try to open stream corresponding to a url.
 * If this fails, remove the last part of the url and try again.
 * This continues until a stream can be opened or the remaining '/' does not
 * contain '/'.
 **/
vector<size_t>
ArchiveReader::ArchiveReaderPrivate::cullName(const string& url,
        InputStream*& stream) const {
    vector<size_t> partpos;
    size_t p = url.rfind('/');
    stream = open(url);
    while (p != string::npos && p != 0 && !stream) {
        stream = open(url.substr(0, p));
        partpos.push_back(p+1);
        p = url.rfind('/', p-1);
    }
    return partpos;
}
/**
 * Obtain a substream provider for which the current stream (currentEntry())
 * points to the stream specified by @p url.
 * On failure, 0 is returned.
 **/
SubStreamProvider*
ArchiveReader::ArchiveReaderPrivate::positionedProvider(const string& url) {
    InputStream* stream = 0;

    // cull the url until a stream can be opened
    vector<size_t> partpos = cullName(url, stream);
    if (!stream) {
        return 0;
    }

    // open the substreams until the complete path has been opened
    SubStreamProvider* provider;
    InputStream* substream = stream;
    vector<size_t>::reverse_iterator i;
    list<StreamPtr> streams;
    streams.push_back(stream);
    for (i = partpos.rbegin(); i != partpos.rend(); ++i) {
        // try to open the stream as a SubStreamProvider
        provider = subStreamProvider(subs, substream, streams);
        if (provider == 0) {
            free(streams);
            return 0;
        }
        // let sn point to the trailing part of the url
        const char* sn = url.c_str() + *i;
        size_t len = url.length();
        bool nextstream = false;
        // try to open the first substream of the current SubStreamProvider
        substream = provider->currentEntry();
        do {
            const EntryInfo& e = provider->entryInfo();
            // check that the filename matches at least one entry
            if (e.type == EntryInfo::File
                    && e.filename.length() < len
                    && strncmp(e.filename.c_str(), sn,
                           e.filename.length()) == 0) {
                nextstream = true;
                // skip the number of entries that are matched
                size_t end = *i + e.filename.length();
                do {
                    ++i;
                } while (i != partpos.rend() && *i < end);
                if (i == partpos.rend()) {
                    // success!
                    openstreams[substream] = streams;
                    return provider;
                }
                // no match: rewind
                --i;
            } else {
                substream = provider->nextEntry();
            }
        } while(substream && !nextstream);
    }
    if (substream) {
        openstreams[substream] = streams;
    } else {
        free(streams);
    }
    return 0;
}
/**
 * Try with each of the streamopeners to open a stream.
 **/
InputStream*
ArchiveReader::ArchiveReaderPrivate::open(const string& url) const {
    InputStream* stream = 0;
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end() && stream == 0; ++i) {
        stream = (*i)->openStream(url);
    }
    return stream;
}
int
ArchiveReader::ArchiveReaderPrivate::localStat(const std::string& url,
        EntryInfo& e) {
    // try with the supplied streamOpeners
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end(); ++i) {
        if ((*i)->stat(url, e) == 0) {
            if (!(e.type & EntryInfo::File)) {
                return 0;
            }

            // check if a ListingInProgress points to this url
            map<string, ListingInProgress*>::const_iterator li =
                listingsInProgress.find(url);
            if (li != listingsInProgress.end()) {
                // use the information in this entry
                e = li->second->root->entry;
                return 0;
            }
 
            // check if this file is in the cache
            map<string, ArchiveEntryCache::RootSubEntry*>::const_iterator se
                = cache.cache.find(url);
            if (se != cache.cache.end()) {
                if (se->second->entry.mtime == e.mtime) {
                    e.type = se->second->entry.type;
                    return 0;
                }
                // the file has changed: it is removed from the cache
                ArchiveEntryCache::RootSubEntry* rse = se->second;
                cache.cache.erase(se->second->entry.filename);
                delete rse;
            }

            // The file exists, but is it an archive?
            InputStream* s = (*i)->openStream(url);
            list<StreamPtr> streams;
            SubStreamProvider* provider = subStreamProvider(subs, s, streams);
            if (provider) {
                // this file contains substreams
                e.type = (EntryInfo::Type)(EntryInfo::Dir|EntryInfo::File);
                free(streams);
/*
                // create an empty entry in the cache
                ArchiveEntryCache::RootSubEntry* rse = cache.cache[url];
                if (rse == NULL) {
                    rse = new ArchiveEntryCache::RootSubEntry();
                    cache.cache[url] = rse;
                }
                rse->indexed = false;
                rse->entry = e;*/
            }
            delete s;
            return 0;
        }
    }
    return -1;
}
ArchiveReader::ArchiveReader() :p(new ArchiveReaderPrivate()) {
}
ArchiveReader::~ArchiveReader() {
    delete p;
}
int
ArchiveReader::stat(const std::string& url, EntryInfo& e) {
    // try to stat the url as a physical file
    if (p->localStat(url, e) == 0) return 0;

    // check the cache (this assumes dirEntries was already called)
    const ArchiveEntryCache::SubEntry* subentry = p->cache.findEntry(url);
    if (subentry) {
        e = subentry->entry;
        return 0;
    }
    // try reading the entries from the collection to which this file belongs
    size_t pos = url.rfind('/');
    if (pos == string::npos) return -1;
    std::string parenturl(url, 0, pos);
    ArchiveReader::DirLister dirlister(dirEntries(parenturl));
    while (dirlister.nextEntry(e)) {
        if (e.filename == url.c_str()+pos+1) {
            return 0;
        }
    }
    return -1;
}
InputStream*
ArchiveReader::openStream(const string& url) {
    InputStream* stream = p->open(url);
    if (stream) return stream;

    // open the substreams until the complete path has been opened
    SubStreamProvider* provider = p->positionedProvider(url);
    if (provider) {
        stream = provider->currentEntry();
    }
    return stream;
}
void
ArchiveReader::addStreamOpener(StreamOpener* opener) {
    p->openers.push_back(opener);
}
void
ArchiveReader::closeStream(InputStream* s) {
    ArchiveReaderPrivate::OpenstreamsType::iterator i(
        p->openstreams.find(s));
    if (i == p->openstreams.end()) {
        delete s;
        return;
    }
    free(i->second);
    p->openstreams.erase(i);
}
bool
ArchiveReader::isArchive(const std::string& url) {
    EntryInfo e;
    if (p->localStat(url, e) != 0) {
        return false;
    }
    return ((e.type & (EntryInfo::File | EntryInfo::Dir)) != 0);
}
std::vector<EntryInfo>
convert(const ArchiveEntryCache::SubEntry* entry) {
    std::vector<EntryInfo> v;
    if (entry == NULL) return v;
    ArchiveEntryCache::SubEntryMap::const_iterator i;
    for (i = entry->entries.begin(); i != entry->entries.end(); ++i) {
        v.push_back(i->second->entry);
    }
    return v;
}
ArchiveReader::DirLister
ArchiveReader::dirEntries(const std::string& url) {
    // find the entry in the cache
    const ArchiveEntryCache::SubEntry* subentry = p->cache.findEntry(url);

    // look for a ListingInProgress
    ListingInProgress* lip = NULL;
    if (subentry == NULL) {
        lip = p->findListingInProgress(url);
    }
    std::vector<EntryInfo> v;
    if (subentry == NULL && lip == NULL) {
        // this entry is not in the cache, we try to open it
        InputStream* s = 0;
        vector<size_t> l = p->cullName(url, s);
        // no entries were found: we return an empty dirlister
        // we have no other way of signaling failure
        // the caller should have checked with stat if the entry is valid
        if (!s) return DirLister(new DirLister::Private(v));

        string name(url);
        if (l.size()) {
            // let name be the name of physical file
            name.resize(l[l.size()-1]-1);
        }
        EntryInfo e;
        // get the properties of the physical file
        p->localStat(name, e);
        lip = new ListingInProgress(p->subs, e, name, s);
        lip->ref();
        p->listingsInProgress[name] = lip;
    }

    if (lip) {
        if (lip->isDone()) {
            p->cache.cache[lip->url] = lip->root;
            lip->root = 0;
            p->listingsInProgress.erase(lip->url);
            if (lip->unref()) delete lip;
        } else {
            return DirLister(new DirLister::Private(lip, url));
        }
    }

    if (subentry == NULL) {
        subentry = p->cache.findEntry(url);
    }
    if (subentry) {
        v = convert(subentry);
    }
    return DirLister(new DirLister::Private(v));
}
bool
ArchiveReader::canHandle(const std::string& url) {
    // remove parts from the back of url until url matches a physical file
    // return true of the physical file is an archive, i.e. contains
    // substreams
    size_t pos = url.rfind('/');
    EntryInfo e;
    int r = p->localStat(url, e);
    while (pos != string::npos && pos != 0 && r == -1) {
        r = p->localStat(url.substr(0, pos), e);
        pos = url.rfind('/', pos-1);
    }
    return r == 0 && e.type & EntryInfo::File && e.type & EntryInfo::Dir;
}
ListingInProgress*
ArchiveReader::ArchiveReaderPrivate::findListingInProgress(const string& url)
        const {
    string n(url);
    size_t p = n.size();
    do {
        map<string, ListingInProgress*>::const_iterator i
            = listingsInProgress.find(n);
        if (i != listingsInProgress.end()) {
            // the root entry is in the map - we are done
            return i->second;
        }
        // remove the last element in the path, and look for that
        p = n.rfind('/');
        if (p != string::npos) {
            n.resize(p);
        }
    } while (p != string::npos);
    // couldn't find it
    return 0;
}
