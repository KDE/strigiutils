#include "archivereader.h"
#include "substreamprovider.h"
#include "tarinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include "mailinputstream.h"
#include "rpminputstream.h"
#include "arinputstream.h"
#include "zipinputstream.h"
#include <vector>
using namespace jstreams;
using namespace std;

bool
DirLister::nextEntry(jstreams::EntryInfo& e) {
    if (pos < entries.size()) {
        e = entries[pos++];
    } else {
        pos = -1;
    }
    return pos != -1;
}
class ArchiveEntryCache {
public:
    class SubEntry {
    public:
        jstreams::EntryInfo entry;
        std::map<std::string, SubEntry> entries;
//        int64_t offset;

        int32_t getCount() const;
    };
    std::map<std::string, SubEntry> cache;
    vector<jstreams::EntryInfo> getEntries(const std::string& url);

    const SubEntry* findEntry(const string& url) const;
    const SubEntry* findRootEntry(const string& url) const;

};
int32_t
ArchiveEntryCache::SubEntry::getCount() const {
    int32_t count = 1;
    map<string, SubEntry>::const_iterator i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        count += i->second.getCount();
    }
    return count;
}
const ArchiveEntryCache::SubEntry*
ArchiveEntryCache::findRootEntry(const string& url) const {
    string n = url;
    const SubEntry* e;
    size_t p = n.size();
    do {
        map<string, SubEntry>::const_iterator i = cache.find(n);
        if (i != cache.end()) {
//            printf("!%s %i\n", n.c_str(), i->second.entries.size());
            return &i->second;
        }
//        printf("^%s\n", n.c_str());
        p = n.rfind('/');
        if (p != string::npos) {
            n = n.substr(0, p);
        }
//        printf("^ %s\n", n.c_str());
    } while (p != string::npos);
    return 0;
}
const ArchiveEntryCache::SubEntry*
ArchiveEntryCache::findEntry(const string& url) const {
    const SubEntry* e = findRootEntry(url);
//    printf("'%s' %p\n", url.c_str(), e);
    if (!e) return e;

    map<string, SubEntry>::const_iterator i;

    size_t p = e->entry.filename.length();
    do {
        if (p == url.length()) {
            return e;
        }
        size_t np = url.find('/', p+1);
        string name;
        if (np == string::npos) {
            name = url.substr(p+1);
        } else {
            name = url.substr(p+1, np-p-1);
        }
//        printf("n %s\n", name.c_str());
        i = e->entries.find(name);
        if (i == e->entries.end()) {
            e = 0;
        } else {
            e = &i->second;
            p = np;
        }
    } while(e && p != string::npos);

    return e;
}
class ArchiveReader::ArchiveReaderPrivate {
public:
    class StreamPtr {
    public:
        jstreams::StreamBase<char>* stream;
        jstreams::SubStreamProvider* provider;
        StreamPtr(jstreams::StreamBase<char>*s) :stream(s), provider(0) {}
        StreamPtr(jstreams::SubStreamProvider*p) :stream(0), provider(p) {}
        void free();
    };
    typedef std::map<jstreams::StreamBase<char>*,
        std::list<StreamPtr> > openstreamsType;
    openstreamsType openstreams;
    std::list<StreamOpener*> openers;
    ArchiveEntryCache cache;
    std::map<bool (*)(const char*, int32_t),
        jstreams::SubStreamProvider* (*)(jstreams::StreamBase<char>*)> subs;

    std::vector<size_t> cullName(const std::string& url,
        jstreams::StreamBase<char>*& stream) const;
    jstreams::SubStreamProvider* getPositionedProvider(const std::string& url,
        bool onlyfiles);
    jstreams::StreamBase<char>* open(const std::string& url) const;
    jstreams::SubStreamProvider* getSubStreamProvider(
        jstreams::StreamBase<char>*, std::list<StreamPtr>& streams);
    static void free(std::list<StreamPtr>& l);
    void fillEntry(ArchiveEntryCache::SubEntry&e, StreamBase<char>*s);
    ArchiveReaderPrivate();
    ~ArchiveReaderPrivate();
};
void
ArchiveReader::ArchiveReaderPrivate::StreamPtr::free() {
    if (stream) delete stream;
    if (provider) delete provider;
}
ArchiveReader::ArchiveReader() {
    p = new ArchiveReaderPrivate();
}
ArchiveReader::~ArchiveReader() {
    delete p;
}
void
ArchiveReader::addStreamOpener(StreamOpener* opener) {
    p->openers.push_back(opener);
}
ArchiveReader::ArchiveReaderPrivate::ArchiveReaderPrivate() {
    subs[MailInputStream::checkHeader] = MailInputStream::factory;
    subs[RpmInputStream::checkHeader ] = RpmInputStream::factory;
    subs[ArInputStream::checkHeader]   = ArInputStream::factory;
    subs[ZipInputStream::checkHeader]  = ZipInputStream::factory;
    subs[TarInputStream::checkHeader]  = TarInputStream::factory;
}
ArchiveReader::ArchiveReaderPrivate::~ArchiveReaderPrivate() {
    if (openstreams.size() > 0) {
        printf("%i streams were not closed.", openstreams.size());
        openstreamsType::iterator i;
        for (i = openstreams.begin(); i != openstreams.end(); ++i) {
            free(i->second);
        }
    }
}
vector<size_t>
ArchiveReader::ArchiveReaderPrivate::cullName(const string& url,
        StreamBase<char>*& stream) const {
    stream = open(url);
    vector<size_t> partpos;
    size_t p = url.rfind('/');
    while (p != string::npos && p != 0 && !stream) {
        stream = open(url.substr(0, p));
        partpos.push_back(p+1);
        p = url.rfind('/', p-1);
    }
    return partpos;
}
SubStreamProvider*
ArchiveReader::ArchiveReaderPrivate::getPositionedProvider(const string& url,
        bool onlyfiles) {
    StreamBase<char>* stream = 0;

    // cull the url until a stream can be opened
    vector<size_t> partpos = cullName(url, stream);
    if (!stream) {
        return 0;
    }

    // open the substreams until we have opened the complete path
    SubStreamProvider* provider;
    StreamBase<char>* substream = stream;
    vector<size_t>::reverse_iterator i;
    list<StreamPtr> streams;
    for (i = partpos.rbegin(); i != partpos.rend(); ++i) {
        const char* sn = url.c_str() + *i;
        size_t len = url.length();
        provider = getSubStreamProvider(substream, streams);
        if (provider == 0) {
            return 0;
        }
        bool nextstream = false;
        substream = provider->currentEntry();
        do {
            const EntryInfo& e = provider->getEntryInfo();
            // check that the filename matches at least one entry
            if (e.type == EntryInfo::File
                    && e.filename.length() < len
                    && strncmp(e.filename.c_str(), sn,
                           e.filename.length()) == 0) {
                nextstream = true;
                // skip the number of entries that are matched
                int end = *i + e.filename.length();
                do {
                    ++i;
                } while (i != partpos.rend() && *i < end);
                if (i == partpos.rend()) {
                    openstreams[substream] = streams;
                    return provider;
                }
                --i;
            }
            substream = provider->nextEntry();
        } while(substream && !nextstream);
    }
    if (substream) {
        openstreams[substream] = streams;
    } else {
        free(streams);
    }
    return 0;
}
StreamBase<char>*
ArchiveReader::openStream(const string& url) {
    StreamBase<char>* stream = p->open(url);
    if (stream) return stream;

    // open the substreams until have opened the complete path
    SubStreamProvider* provider = p->getPositionedProvider(url, true);
    if (provider) {
        stream = provider->currentEntry();
    }
    return stream;
}
void
ArchiveReader::closeStream(jstreams::StreamBase<char>* s) {
    ArchiveReaderPrivate::openstreamsType::iterator i = p->openstreams.find(s);
    if (i == p->openstreams.end()) return;
    p->free(i->second);
    p->openstreams.erase(i);
}
void
ArchiveReader::ArchiveReaderPrivate::free(list<StreamPtr>& l) {
    list<StreamPtr>::iterator i;
    for (i=l.begin(); i!=l.end(); ++i) {
        i->free();
    }
}
StreamBase<char>*
ArchiveReader::ArchiveReaderPrivate::open(const string& url) const {
    StreamBase<char>* stream = 0;
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end() && stream == 0; ++i) {
        stream = (*i)->openStream(url);
    }
    return stream;
}
SubStreamProvider*
ArchiveReader::ArchiveReaderPrivate::getSubStreamProvider(
        StreamBase<char>* input, list<ArchiveReaderPrivate::StreamPtr>& streams
        ) {
    if (input == 0) return 0;
    StreamBase<char>* s = new BZ2InputStream(input);
    if (s->getStatus() == Ok) {
        streams.push_back(s);
    } else {
        //printf("no bz2\n", input);
        delete s;
        input->reset(0);
        s = new GZipInputStream(input);
        if (s->getStatus() != Ok) {
            //printf("no gz\n", input);
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
        SubStreamProvider* (*)(StreamBase<char>*)>::const_iterator i;
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
    
    return 0;
}
bool
ArchiveReader::isArchive(const std::string& url) {
    list<StreamOpener*>::const_iterator i;
    SubStreamProvider* provider = 0;
    EntryInfo e;
    for (i = p->openers.begin(); provider == 0 && i != p->openers.end(); ++i) {
        if ((*i)->stat(url, e) != -1) {
            if (e.type & EntryInfo::Dir) {
                return false;
            }
            // we statted, now we'd like to know if this file has subentries
            StreamBase<char>* s = (*i)->openStream(url);
            list<ArchiveReaderPrivate::StreamPtr> streams;
            provider = p->getSubStreamProvider(s, streams);
            if (provider) {
                e.type = EntryInfo::Dir;
                p->free(streams);
            }
            delete s;
        }
    }
    return provider != 0;
}
int
ArchiveReader::stat(const std::string& url, jstreams::EntryInfo& e) {
    // try with the supplied streamOpeners
    list<StreamOpener*>::const_iterator i;
    for (i = p->openers.begin(); i != p->openers.end(); ++i) {
        if ((*i)->stat(url, e) != -1) {
            if (e.type & EntryInfo::Dir) {
                return 0;
            }
            // we statted, now we'd like to know if this file has subentries
            StreamBase<char>* s = (*i)->openStream(url);
            list<ArchiveReaderPrivate::StreamPtr> streams;
            SubStreamProvider* provider = p->getSubStreamProvider(s, streams);
            if (provider) {
                e.type = EntryInfo::Dir;
                p->free(streams);
            }
            delete s;
            return 0;
        }
    }

    // check the cache (this assumes getDirEntries was already called)
    const ArchiveEntryCache::SubEntry *subentry = p->cache.findEntry(url);
    if (subentry) {
        printf("stat %s %i\n", url.c_str(), subentry->entry.type);
        e = subentry->entry;
        return 0;
    }
    return -1;
}
void
addEntry(ArchiveEntryCache::SubEntry& e, ArchiveEntryCache::SubEntry& se) {
    // split path into components
    vector<string> names;
    string name = se.entry.filename;
//    printf("%s -> %s\n", e.entry.filename.c_str(), name.c_str());
    size_t p = name.find('/');
    while (p != string::npos) {
        names.push_back(name.substr(0, p));
        name = name.substr(p+1);
        p = name.find('/');
    }
    names.push_back(name);
    se.entry.filename = name;

    // find the right entry
    map<string, ArchiveEntryCache::SubEntry>::iterator ii;
    ArchiveEntryCache::SubEntry* parent = &e;
    for (int i=0; i<names.size(); ++i) {
//        printf(" -> %s\n", names[i].c_str());
        ii = parent->entries.find(names[i]);
        if (ii == parent->entries.end()) {
            ArchiveEntryCache::SubEntry newse;
            newse.entry.filename = names[i];
            newse.entry.type = EntryInfo::Dir;
            newse.entry.size = 0;
            parent->entries[names[i]] = newse;
            ii = parent->entries.find(names[i]);
        }
        parent = &ii->second;
    }
    *parent = se;
}
void
ArchiveReader::ArchiveReaderPrivate::fillEntry(ArchiveEntryCache::SubEntry& e,
       StreamBase<char>* s) {
//    printf("fillentry\n");
    list<StreamPtr> streams;
    SubStreamProvider* p = getSubStreamProvider(s, streams);
    if (!p) return;
    do {
        ArchiveEntryCache::SubEntry se;
        se.entry = p->getEntryInfo();
//        printf(">%s\n", se.entry.filename.c_str());
        fillEntry(se, p->currentEntry());
        if (se.entry.size < 0) {
            // read entire stream to determine it's size
            StreamBase<char> *es = p->currentEntry();
            const char* c;
            while (es->read(c, 1, 0) > 0) {}
            se.entry.size = es->getSize();
            if (se.entry.size < 0) se.entry.size = 0;
        }
        e.entry.type = (EntryInfo::Type)(e.entry.type|EntryInfo::Dir);
        addEntry(e, se);
    } while (p->nextEntry());
    free(streams);
}
DirLister
ArchiveReader::getDirEntries(const std::string& url) {
    std::vector<jstreams::EntryInfo> v;
    // find the entry in the cache
    const ArchiveEntryCache::SubEntry *subentry = p->cache.findEntry(url);
//    printf("dir %s %p\n", url.c_str(), subentry);
    string name;
    if (subentry) {
        name = subentry->entry.filename;
    } else {
        // or create a new entry
        StreamBase<char>* s = 0;
        vector<size_t> l = p->cullName(url, s);
        if (!s) return DirLister(v);
        if (l.size()) {
            name = url.substr(0, l[l.size()-1]-1);
        } else {
            name = url;
        }
        EntryInfo e;
        e.filename = name;
        ArchiveEntryCache::SubEntry se;
        se.entry = e;
        p->fillEntry(se, s);
        delete s;
        p->cache.cache[name] = se;
        subentry = p->cache.findEntry(url);
    }

    if (subentry) {
        map<string, ArchiveEntryCache::SubEntry>::const_iterator i;
        for (i = subentry->entries.begin(); i != subentry->entries.end(); ++i) {
            v.push_back(i->second.entry);
        }
    }
    DirLister dl(v);
    return dl;
}
