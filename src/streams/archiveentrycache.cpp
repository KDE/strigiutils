#include "archiveentrycache.h"
#include <iostream>

using namespace std;

ArchiveEntryCache::SubEntry::~SubEntry() {
    SubEntryMap::iterator i;
    for (i=entries.begin(); i!=entries.end(); ++i) {
        delete i->second;
    }
}
map<string, ArchiveEntryCache::RootSubEntry*>::const_iterator
ArchiveEntryCache::findRootEntry(const string& url) const {
    string n(url);
    size_t p = n.size();
    do {
        map<string, RootSubEntry*>::const_iterator i = cache.find(n);
        if (i != cache.end()) {
            // the root entry is in the cache - we're done
            return i;
        }
        // remove the last element in the path, and look for that
        p = n.rfind('/');
        if (p != string::npos) {
            n.resize(p);
        }
    } while (p != string::npos);
    // couldn't find it
    return cache.end();
}
/**
 * Find the entry corresponding to @p url.
 **/
const ArchiveEntryCache::SubEntry*
ArchiveEntryCache::findEntry(const string& url) const {
    // find the root entry that should contain the wanted entry
    map<string, RootSubEntry*>::const_iterator ei = findRootEntry(url);
    if (ei == cache.end()) return 0; // no root could be find
    if (ei->first == url) {
        // the requested entry is a root entry: we are done
        return ei->second;
    }
    return ei->second->findEntry(ei->first, url);
}
const ArchiveEntryCache::SubEntry*
ArchiveEntryCache::RootSubEntry::findEntry(const std::string& rootpath,
        const std::string& url) const {
    // use the path components as keys to find the entry
    const SubEntry* e = this;
    size_t p = rootpath.length();
    string name;
    do {
        size_t np = url.find('/', p+1);
        if (np == string::npos) {
            name.assign(url, p+1, url.size());
        } else {
            name.assign(url, p+1, np-p-1);
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
