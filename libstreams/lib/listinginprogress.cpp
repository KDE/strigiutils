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
#include "listinginprogress.h"
#include <strigi/gzipinputstream.h>
#include <strigi/bz2inputstream.h>
#include <strigi/lzmainputstream.h>
#include <iostream>

using namespace Strigi;
using namespace std;

namespace {
/**
 * Add the entry @p se to the cache.
 * The filename of @p se contains the complete path.
 * This is split on '/' into its parts. The parts are used as keys to find
 * the parents of the new entry.
 * Any entry that is not yet in the cache is created as a directory.
 **/
void
addEntry(ArchiveEntryCache::SubEntry* parent,
        ArchiveEntryCache::SubEntry* se) {
    // split path into components, the components are placed in the array
    // 'names'. The filename is kept in the filename member of se
    vector<string> names;
    string& name = se->entry.filename;
    string::size_type p = name.find('/');
    while (p != string::npos) {
        names.push_back(name.substr(0, p));
        name.erase(0, p + 1);
        p = name.find('/');
    }

    // find the right entry
    ArchiveEntryCache::SubEntry* child;
    for (size_t i=0; i<names.size(); ++i) {
        child = parent->entries[names[i]];
        if (child == NULL) {
            // create a new directory entry if it is not yet in the cache
            child = new ArchiveEntryCache::SubEntry();
            child->entry.filename = names[i];
            child->entry.type = EntryInfo::Dir;
            child->entry.size = 0;
            parent->entries[names[i]] = child;
        }
        parent = child;
    }
    // this is what we came for: add the entry to the parent
    parent->entries[name] = se;
}
}

void
free(list<StreamPtr>& l) {
    list<StreamPtr>::iterator i;
    for (i=l.begin(); i!=l.end(); ++i) {
        i->free();
    }
    l.clear();
}

SubStreamProvider*
subStreamProvider(const Subs& subs, InputStream* input,
         list<StreamPtr>& streams) {
    if (input == 0) return 0;
    InputStream* s = input;

    bool foundCompressedStream;
    int nestingDepth = 0;
    do {
        foundCompressedStream = false;
        // check if this is a compressed stream
        const char* c;
        int32_t n = s->read(c, 16, 0);
        s->reset(0);
        if (BZ2InputStream::checkHeader(c, n)) {
            InputStream* ns = new BZ2InputStream(s);
            if (ns->status() == Ok) {
                foundCompressedStream = true;
                s = ns;
                streams.push_back(s);
            } else {
                delete ns;
                s->reset(0);
            }
        }
        n = s->read(c, 2, 0);
        s->reset(0);
        if (n >= 2 && c[0] == 0x1f && c[1] == (char)0x8b) {
            InputStream* ns = new GZipInputStream(s);
            if (ns->status() == Ok) {
                foundCompressedStream = true;
                s = ns;
                streams.push_back(s);
            } else {
                delete ns;
                s->reset(0);
            }
        }
        n = s->read(c, 2, 0);
        s->reset(0);
        if (LZMAInputStream::checkHeader(c, n)) {
            InputStream* ns = new LZMAInputStream(s);
            if (ns->status() == Ok) {
                foundCompressedStream = true;
                s = ns;
                streams.push_back(s);
            } else {
                delete ns;
                s->reset(0);
            }
        }
    } while (foundCompressedStream && nestingDepth++ < 32);
 
    const char* c;
    int32_t n = s->read(c, 1024, 0);
    s->reset(0);
    SubStreamProvider* ss;
    map<bool (*)(const char*, int32_t),
        SubStreamProvider* (*)(InputStream*)>::const_iterator i;
    for (i = subs.begin(); i != subs.end(); ++i) {
        // check if the header matches for each substreamprovider
        if (i->first(c, n)) {
            // create a new SubStreamProvider
            ss = i->second(s);
            if (ss->nextEntry()) {
                streams.push_back(ss);
                // return the first substream
                return ss;
            }
            // even though the header was good, this stream has no substream
            delete ss;
            s->reset(0);
            n = s->read(c, 1, 0);
            s->reset(0);
        }
    }
    free(streams);
    return 0;
}

ListingInProgress::ListingInProgress(const Subs& sbs,
        const EntryInfo& entry, const std::string& u, InputStream* s)
        :subs(sbs), stream(s), refcount(0), url(u) {
    root = new ArchiveEntryCache::RootSubEntry();
    root->entry = entry;
    root->indexed = true;
    stack.resize(10);
    StackEntry& e = stack[0];
    e.entry = root;
    e.p = subStreamProvider(subs, s, e.streams);
    if (e.p) {
        e.entry->entry.type
                = (EntryInfo::Type)(e.entry->entry.type|EntryInfo::Dir);
        currentdepth = 0;
    } else {
        currentdepth = -1;
    }
}
ListingInProgress::~ListingInProgress() {
    for (size_t i=0; i<stack.size(); ++i) {
        free(stack[i].streams);
    }
    delete root;
    delete stream;
}
void
ListingInProgress::fillEntry(InputStream* s) {
    stack.resize(10);
    StackEntry& e = stack[0];
    e.entry = root;
    e.p = subStreamProvider(subs, s, e.streams);
    if (e.p) {
        e.entry->entry.type
                = (EntryInfo::Type)(e.entry->entry.type|EntryInfo::Dir);
        int depth = 0;
        do {
            depth = nextEntry(depth);
        } while (depth >= 0);
    }
}
int
ListingInProgress::nextEntry(int depth) {
    if (stack.size() < (size_t)(depth+1)) {
        stack.resize(depth+1);
    }
    StackEntry* e = &stack[depth];
    StackEntry* ce = &stack[depth+1];
    if (e->p) {
        // create a child entry
        ce->entry = new ArchiveEntryCache::SubEntry();
        ce->entry->entry = e->p->entryInfo();
        ce->p = subStreamProvider(subs, e->p->currentEntry(), ce->streams);
        if (ce->p) {
            // the child has substreams too
            ce->entry->entry.type
                = (EntryInfo::Type)(ce->entry->entry.type|EntryInfo::Dir);
            return nextEntry(depth+1);
        }
    } else if (depth--) {
        ce = e;
        e = &stack[depth];
    }
    if (depth >= 0) {
        if (ce->entry->entry.size < 0) {
            // read entire stream to determine it's size
            InputStream *es = e->p->currentEntry();
            const char* c;
            while (es->read(c, 1, 0) > 0) {}
            ce->entry->entry.size = max(es->size(), (int64_t)0);
        }
        addEntry(e->entry, ce->entry);
        if (!e->p->nextEntry()) {
            free(e->streams);
            e->p = 0;
        }
    }
    return depth;
}
bool
ListingInProgress::nextEntry() {
    if (currentdepth >= 0) {
        currentdepth = nextEntry(currentdepth);
    }
    if (currentdepth < 0) {
        delete stream;
        stream = 0;
    }
    return currentdepth >= 0;
}
const ArchiveEntryCache::SubEntry*
ListingInProgress::nextEntry(const std::string& url) {
    if (url == this->url) {
        return (root->entries.size() || nextEntry(root)) ?root :NULL;
    }
    const ArchiveEntryCache::SubEntry* entry = root->findEntry(this->url, url);
    bool ok = true;
    while (ok && (entry == NULL || entry->entries.size() == 0)) {
        ok = nextEntry();
        entry = root->findEntry(this->url, url);
    }
    return entry;
}
bool
ListingInProgress::nextEntry(const ArchiveEntryCache::SubEntry* entry) {
    if (currentdepth < 0) return false;
    size_t initialSize = entry->entries.size();
    bool ok;
    do {
        ok = nextEntry();
    } while (ok && initialSize == entry->entries.size());
    return entry->entries.size() > initialSize;
}
bool
ListingInProgress::isDone() const {
    return stream == NULL;
}
