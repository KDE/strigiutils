/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "archivereader.h"
#include <strigi/strigiconfig.h>
#include "streambase.h"
#include "substreamprovider.h"
#include "sdfinputstream.h"
#include "tarinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include "mailinputstream.h"
#include "rpminputstream.h"
#include "arinputstream.h"
#include "zipinputstream.h"
#include <vector>
#include <iostream>
#include <cstring>

using namespace std;
using namespace Strigi;

class ArchiveReader::DirLister::Private {
public:
    int pos;
    const vector<EntryInfo> entries;
    Private(const vector<EntryInfo>& v) :entries(v) {
        pos = 0;
    }
    bool
    nextEntry(EntryInfo& e) {
        if (pos >= 0 && pos < (int)entries.size()) {
            e = entries[pos++];
        } else {
            pos = -1;
        }
        return pos != -1;
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
        EntryInfo entry;
        /** A list of subentries of this entry. */
        //can't define staticly constructed object while object is being defined
        SubEntryMap entries;
        /**
         * @brief The number of entries in the cache below and including
         * this one.
         *
         * @return the size of the tree rooted at this node
         */
        int32_t count() const;
        /** Constructor */
        SubEntry() { }
        /** Destructor */
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
    };

    /**
     * @brief The cache itself.
     *
     * Contains all the root entries in the cache.
     */
    std::map<std::string, RootSubEntry> cache;

    /**
     * @brief Find an entry in the cache by its URL
     *
     * @param url the url of the entry
     * @return a pointer to the entry, if it is in the cache,
     * 0 if the entry is not in the cache
     */
    const SubEntry* findEntry(const string& url) const;
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
    map<string, RootSubEntry>::const_iterator findRootEntry(const string& url)
        const;
    /**
     * @brief Print a text representation of the cache to stdout.
     * For debugging purposes.
     */
    void print() const;
};

ArchiveEntryCache::SubEntry::~SubEntry() {
    SubEntryMap::iterator i;
    for (i=entries.begin(); i!=entries.end(); ++i) {
        delete i->second;
    }
    // Probably superfluous
    entries.clear();
}

void
ArchiveEntryCache::print() const {
    std::map<std::string, RootSubEntry>::const_iterator j;
    for (j=cache.begin(); j!=cache.end(); ++j) {
        printf("x %s\n", j->first.c_str());
        SubEntryMap::const_iterator i;
        for (i = j->second.entries.begin(); i != j->second.entries.end(); ++i) {
            printf("- %s ", i->second->entry.filename.c_str());
        }
        printf("\n");
    }
}
int32_t
ArchiveEntryCache::SubEntry::count() const {
    int32_t count = 1;
    SubEntryMap::const_iterator i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        count += i->second->count();
    }
    return count;
}
map<string, ArchiveEntryCache::RootSubEntry>::const_iterator
ArchiveEntryCache::findRootEntry(const string& url) const {
    string n = url;
    size_t p = n.size();
    do {
        map<string, RootSubEntry>::const_iterator i = cache.find(n);
        if (i != cache.end()) {
            // it's a root entry in the cache - we're done
            return i;
        }
        // remove the last element in the path, and look for that
        p = n.rfind('/');
        if (p != string::npos) {
            n = n.substr(0, p);
        }
    } while (p != string::npos);
    // couldn't find it
    return cache.end();
}
const ArchiveEntryCache::SubEntry*
ArchiveEntryCache::findEntry(const string& url) const {
    map<string, RootSubEntry>::const_iterator ei = findRootEntry(url);
    if (ei == cache.end()) return 0;
    if (ei->first == url) {
        return &ei->second;
    }
    const SubEntry* e = &ei->second;

    size_t p = ei->first.length();
    string name;
    do {
        size_t np = url.find('/', p+1);
        if (np == string::npos) {
            name.assign(url.substr(p+1));
        } else {
            name.assign(url.substr(p+1, np-p-1));
        }
        SubEntryMap::const_iterator i = e->entries.find(name);
        if (i == e->entries.end()) {
            e = 0;
        } else {
            e = i->second;
            p = np;
        }
        if (p == url.length()) {
            return e;
        }
    } while(e && p != string::npos);

    return e;
}


/** Private data members and functions for ArchiveReader */
class ArchiveReader::ArchiveReaderPrivate {
public:
    class StreamPtr {
    public:
        InputStream* stream;
        SubStreamProvider* provider;
        StreamPtr() :stream(0), provider(0) {}
        StreamPtr(InputStream* s) :stream(s), provider(0) {}
        StreamPtr(SubStreamProvider* p) :stream(0), provider(p) {}
        void free();
    };
    typedef std::map<InputStream*,
        std::list<StreamPtr> > openstreamsType;
    openstreamsType openstreams;
    std::list<StreamOpener*> openers;
    ArchiveEntryCache cache;
    std::map<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)> subs;

    std::vector<size_t> cullName(const std::string& url,
        InputStream*& stream) const;
    SubStreamProvider* positionedProvider(const std::string& url,
        bool onlyfiles);
    InputStream* open(const std::string& url) const;
    SubStreamProvider* subStreamProvider(
        InputStream*, std::list<StreamPtr>& streams);
    static void free(std::list<StreamPtr>& l);
    int fillEntry(ArchiveEntryCache::SubEntry&e, InputStream*s);
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
};
void
ArchiveReader::ArchiveReaderPrivate::StreamPtr::free() {
    if (stream) delete stream;
    if (provider) delete provider;
}
void
addEntry(ArchiveEntryCache::SubEntry* e, ArchiveEntryCache::SubEntry* se) {
    // split path into components
    vector<string> names;
    string name = se->entry.filename;
    string::size_type p = name.find('/');
    while (p != string::npos) {
        names.push_back(name.substr(0, p));
        name = name.substr(p + 1);
        p = name.find('/');
    }
    se->entry.filename = name;

    // find the right entry
    ArchiveEntryCache::SubEntryMap::iterator ii;
    ArchiveEntryCache::SubEntry* parent = e;
    for (uint i=0; i<names.size(); ++i) {
        ii = parent->entries.find(names[i]);
        if (ii == parent->entries.end()) {
            ArchiveEntryCache::SubEntry *newse
                = new ArchiveEntryCache::SubEntry();
            newse->entry.filename = names[i];
            newse->entry.type = EntryInfo::Dir;
            newse->entry.size = 0;
            parent->entries[names[i]] = newse;
            ii = parent->entries.find(names[i]);
        }
        parent = ii->second;
    }
    parent->entries[name] = se;
}
ArchiveReader::ArchiveReaderPrivate::ArchiveReaderPrivate() {
    typedef std::pair<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)> SubsPair;

    subs.insert(SubsPair(MailInputStream::checkHeader,MailInputStream::factory) );

    subs.insert(SubsPair(RpmInputStream::checkHeader,RpmInputStream::factory) );
    subs.insert(SubsPair(ArInputStream::checkHeader,ArInputStream::factory) );
    subs.insert(SubsPair(ZipInputStream::checkHeader,ZipInputStream::factory) );
    subs.insert(SubsPair(TarInputStream::checkHeader,TarInputStream::factory) );
    subs.insert(SubsPair(SdfInputStream::checkHeader,SdfInputStream::factory) );
}
ArchiveReader::ArchiveReaderPrivate::~ArchiveReaderPrivate() {
    if (openstreams.size() > 0) {
        cerr << openstreams.size() << " streams were not closed." << endl;
        openstreamsType::iterator i;
        for (i = openstreams.begin(); i != openstreams.end(); ++i) {
            free(i->second);
        }
    }
}
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
SubStreamProvider*
ArchiveReader::ArchiveReaderPrivate::positionedProvider(const string& url,
        bool onlyfiles) {
    InputStream* stream = 0;

    // cull the url until a stream can be opened
    vector<size_t> partpos = cullName(url, stream);
    if (!stream) {
        return 0;
    }

    // open the substreams until we have opened the complete path
    SubStreamProvider* provider;
    InputStream* substream = stream;
    vector<size_t>::reverse_iterator i;
    list<StreamPtr> streams;
    streams.push_back(stream);
    for (i = partpos.rbegin(); i != partpos.rend(); ++i) {
        const char* sn = url.c_str() + *i;
        size_t len = url.length();
        provider = subStreamProvider(substream, streams);
        if (provider == 0) {
            return 0;
        }
        bool nextstream = false;
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
                uint end = *i + e.filename.length();
                do {
                    ++i;
                } while (i != partpos.rend() && *i < end);
                if (i == partpos.rend()) {
                    openstreams[substream] = streams;
                    return provider;
                }
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
void
ArchiveReader::ArchiveReaderPrivate::free(list<StreamPtr>& l) {
    list<StreamPtr>::iterator i;
    for (i=l.begin(); i!=l.end(); ++i) {
        i->free();
    }
}
InputStream*
ArchiveReader::ArchiveReaderPrivate::open(const string& url) const {
    InputStream* stream = 0;
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end() && stream == 0; ++i) {
        stream = (*i)->openStream(url);
    }
    return stream;
}
SubStreamProvider*
ArchiveReader::ArchiveReaderPrivate::subStreamProvider(
        InputStream* input, list<ArchiveReaderPrivate::StreamPtr>& streams
        ) {
    if (input == 0) return 0;
    InputStream* s = new BZ2InputStream(input);
    if (s->status() == Ok) {
        streams.push_back(s);
    } else {
        delete s;
        input->reset(0);
        s = new GZipInputStream(input);
        if (s->status() != Ok) {
            delete s;
            input->reset(0);
            s = input;
        } else {
            streams.push_back(s);
        }
    }
    const char* c;
    int32_t n = s->read(c, 1024, 0);
    s->reset(0);
    SubStreamProvider* ss;
    map<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)>::const_iterator i;
    for (i = subs.begin(); i != subs.end(); ++i) {
        if (i->first(c,n)) {
            ss = i->second(s);
            if (ss->nextEntry()) {
                streams.push_back(ss);
                return ss;
            }
            delete ss;
            s->reset(0);
            n = s->read(c, 1, 0);
            s->reset(0);
        }
    }
    free(streams);
    return 0;
}
int
ArchiveReader::ArchiveReaderPrivate::fillEntry(ArchiveEntryCache::SubEntry& e,
       InputStream* s) {
    int nentries = 0;
    list<StreamPtr> streams;
    SubStreamProvider* p = subStreamProvider(s, streams);
    if (!p) return 0;
    do {
        ArchiveEntryCache::SubEntry* se = new ArchiveEntryCache::SubEntry();
        se->entry = p->entryInfo();
        int nsubentries = fillEntry(*se, p->currentEntry());
        if (se->entry.size < 0) {
            // read entire stream to determine it's size
            InputStream *es = p->currentEntry();
            const char* c;
            while (es->read(c, 1, 0) > 0) {}
            se->entry.size = max(es->size(), (int64_t)0);
        }
        addEntry(&e, se);
        if (nsubentries) {
            nentries += nsubentries;
        }
        nentries++;
    } while (p->nextEntry());
    free(streams);
    if (nentries) {
        e.entry.type = (EntryInfo::Type)(e.entry.type|EntryInfo::Dir);
    }
    return nentries;
}
int
ArchiveReader::ArchiveReaderPrivate::localStat(const std::string& url, EntryInfo& e) {
    // try with the supplied streamOpeners
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end(); ++i) {
        if ((*i)->stat(url, e) == 0) {
            if (!(e.type & EntryInfo::File)) {
                return 0;
            }
            // check if this file is in the cache
            map<string, ArchiveEntryCache::RootSubEntry>::const_iterator se
                = cache.findRootEntry(url);
            if (se != cache.cache.end()) {
                if (se->second.entry.mtime == e.mtime) {
                    e.type = se->second.entry.type;
                    return 0;
                }
                cache.cache.erase(se->second.entry.filename);
            }

            // we statted, now we'd like to know if this file has subentries
            InputStream* s = (*i)->openStream(url);
            list<ArchiveReaderPrivate::StreamPtr> streams;
            SubStreamProvider* provider = subStreamProvider(s, streams);
            if (provider) {
                e.type = (EntryInfo::Type)(EntryInfo::Dir|EntryInfo::File);
                free(streams);

                ArchiveEntryCache::RootSubEntry& rse = cache.cache[url];
                rse.indexed = false;
                rse.entry = e;
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
    if (p->localStat(url, e) == 0) return 0;

    // check the cache (this assumes dirEntries was already called)
    const ArchiveEntryCache::SubEntry* subentry = p->cache.findEntry(url);
    if (subentry) {
        e = subentry->entry;
        return 0;
    }
    // try reading the entries from the collection to which this file belongs
    dirEntries(url);
    subentry = p->cache.findEntry(url);
    if (subentry) {
        e = subentry->entry;
        return 0;
    }
    return -1;
}
InputStream*
ArchiveReader::openStream(const string& url) {
    InputStream* stream = p->open(url);
    if (stream) return stream;

    // open the substreams until have opened the complete path
    SubStreamProvider* provider = p->positionedProvider(url, true);
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
    ArchiveReaderPrivate::openstreamsType::iterator i = p->openstreams.find(s);
    if (i == p->openstreams.end()) {
        delete s;
        return;
    }
    p->free(i->second);
    p->openstreams.erase(i);
}
bool
ArchiveReader::isArchive(const std::string& url) {
    EntryInfo e;
    if (p->localStat(url, e) != 0) {
        return false;
    }
    return e.type & EntryInfo::File && e.type & EntryInfo::Dir;
}
ArchiveReader::DirLister
ArchiveReader::dirEntries(const std::string& url) {
    std::vector<EntryInfo> v;

    // find the entry in the cache
    const ArchiveEntryCache::SubEntry *subentry = p->cache.findEntry(url);

    // if this is a root entry that was not yet indexed, index it now
    if (subentry) {
        map<string, ArchiveEntryCache::RootSubEntry>::const_iterator se
            = p->cache.findRootEntry(url);
        if (se != p->cache.cache.end() && !se->second.indexed) {
            // pretend like it's not in the cache
            subentry = 0;
        }
    }
    string name;
    if (subentry) {
        name = subentry->entry.filename;
    } else {
        // or create a new entry
        InputStream* s = 0;
        vector<size_t> l = p->cullName(url, s);
        if (!s) return DirLister(new DirLister::Private(v));
        if (l.size()) {
            name = url.substr(0, l[l.size()-1]-1);
        } else {
            name = url;
        }
        EntryInfo e;
        p->localStat(name, e);
        ArchiveEntryCache::RootSubEntry& se = p->cache.cache[name];
        se.indexed = true;
        se.entry = e;
        p->fillEntry(se, s);
        delete s;
        subentry = p->cache.findEntry(url);
    }

    if (subentry) {
        ArchiveEntryCache::SubEntryMap::const_iterator i;
        for (i = subentry->entries.begin(); i != subentry->entries.end(); ++i) {
            v.push_back(i->second->entry);
        }
    }
    DirLister dl(new DirLister::Private(v));
    return dl;
}
bool
ArchiveReader::canHandle(const std::string& url) {
    vector<size_t> partpos;
    size_t pos = url.rfind('/');
    EntryInfo e;
    int r = p->localStat(url, e);
    while (pos != string::npos && pos != 0 && r == -1) {
        r = p->localStat(url.substr(0, pos), e);
        partpos.push_back(pos+1);
        pos = url.rfind('/', pos-1);
    }
    return r == 0 && e.type & EntryInfo::File && e.type & EntryInfo::Dir;
}
