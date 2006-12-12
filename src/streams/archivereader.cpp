#include "jstreamsconfig.h"
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
    if (pos < (int)entries.size()) {
        e = entries[pos++];
    } else {
        pos = -1;
    }
    return pos != -1;
}
class ArchiveEntryCache {
public:
    class SubEntry {
    private:
//        SubEntry(const SubEntry&);
//        void operator=(const SubEntry&);
    public:
        jstreams::EntryInfo entry;
        //can't define staticly constructed object while object is being defined
        std::map<std::string, SubEntry> entries;
        int32_t getCount() const;
        SubEntry() {
//            entries = new std::map<std::string, SubEntry>;
        }
        virtual ~SubEntry() {
//            printf("ms %i\n", entries.size());
//            delete entries;
        };
    };
    class RootSubEntry : public SubEntry {
    private:
//        RootSubEntry(const RootSubEntry&);
//        void operator=(const RootSubEntry&);
    public:
        RootSubEntry() :SubEntry() {}
        bool indexed;
    };

    std::map<std::string, RootSubEntry> cache;
    vector<jstreams::EntryInfo> getEntries(const std::string& url);

    const SubEntry* findEntry(const string& url) const;
    map<string, RootSubEntry>::const_iterator findRootEntry(const string& url)
        const;
    void print() const;
};

void
ArchiveEntryCache::print() const {
    std::map<std::string, RootSubEntry>::const_iterator j;
    for (j=cache.begin(); j!=cache.end(); ++j) {
        printf("x %s\n", j->first.c_str());
        std::map<std::string, SubEntry>::const_iterator i;
        for (i = j->second.entries.begin(); i != j->second.entries.end(); ++i) {
            printf("- %s ", i->second.entry.filename.c_str());
        }
        printf("\n");
    }
}
int32_t
ArchiveEntryCache::SubEntry::getCount() const {
    int32_t count = 1;
    map<string, SubEntry>::const_iterator i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        count += i->second.getCount();
    }
    return count;
}
map<string, ArchiveEntryCache::RootSubEntry>::const_iterator
ArchiveEntryCache::findRootEntry(const string& url) const {
    string n = url;
    //const SubEntry* e;
    size_t p = n.size();
    do {
        map<string, RootSubEntry>::const_iterator i = cache.find(n);
        if (i != cache.end()) {
            return i;
        }
        p = n.rfind('/');
        if (p != string::npos) {
            n = n.substr(0, p);
        }
    } while (p != string::npos);
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

    map<string, SubEntry>::const_iterator i;

    size_t p = ei->first.length();
    do {
        size_t np = url.find('/', p+1);
        string name;
        if (np == string::npos) {
            name = url.substr(p+1);
        } else {
            name = url.substr(p+1, np-p-1);
        }
        i = e->entries.find(name);
        if (i == e->entries.end()) {
            e = 0;
        } else {
            e = &i->second;
            p = np;
        }
        if (p == url.length()) {
            return e;
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
        StreamPtr() :stream(0), provider(0) {}
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
    int fillEntry(ArchiveEntryCache::SubEntry&e, StreamBase<char>*s);
    ArchiveReaderPrivate();
    ~ArchiveReaderPrivate();
};
void
ArchiveReader::ArchiveReaderPrivate::StreamPtr::free() {
    if (stream) delete stream;
    if (provider) delete provider;
}
void
addEntry(ArchiveEntryCache::SubEntry& e, ArchiveEntryCache::SubEntry& se) {
    // split path into components
    vector<string> names;
    string name = se.entry.filename;
    string::size_type p = name.find('/');
    while (p != string::npos) {
        names.push_back(name.substr(0, p));
        name = name.substr(p + 1);
        p = name.find('/');
    }
    names.push_back(name);
    se.entry.filename = name;

    // find the right entry
    map<string, ArchiveEntryCache::SubEntry>::iterator ii;
    ArchiveEntryCache::SubEntry* parent = &e;
    for (uint i=0; i<names.size(); ++i) {
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
ArchiveReader::ArchiveReaderPrivate::ArchiveReaderPrivate() {
    typedef std::pair<bool (*)(const char*, int32_t),
        jstreams::SubStreamProvider* (*)(jstreams::StreamBase<char>*)> SubsPair;

    subs.insert(SubsPair(MailInputStream::checkHeader,MailInputStream::factory) );

    subs.insert(SubsPair(RpmInputStream::checkHeader,RpmInputStream::factory) );
    subs.insert(SubsPair(ArInputStream::checkHeader,ArInputStream::factory) );
    subs.insert(SubsPair(ZipInputStream::checkHeader,ZipInputStream::factory) );
    subs.insert(SubsPair(TarInputStream::checkHeader,TarInputStream::factory) );
}
ArchiveReader::ArchiveReaderPrivate::~ArchiveReaderPrivate() {
    if (openstreams.size() > 0) {
        fprintf(stderr, "%i streams were not closed.\n", openstreams.size());
        openstreamsType::iterator i;
        for (i = openstreams.begin(); i != openstreams.end(); ++i) {
            free(i->second);
        }
    }
}
vector<size_t>
ArchiveReader::ArchiveReaderPrivate::cullName(const string& url,
        StreamBase<char>*& stream) const {
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
    streams.push_back(stream);
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
        delete s;
        input->reset(0);
        s = new GZipInputStream(input);
        if (s->getStatus() != Ok) {
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
int
ArchiveReader::ArchiveReaderPrivate::fillEntry(ArchiveEntryCache::SubEntry& e,
       StreamBase<char>* s) {
    int nentries = 0;
    list<StreamPtr> streams;
    SubStreamProvider* p = getSubStreamProvider(s, streams);
    if (!p) return 0;
    do {
        ArchiveEntryCache::SubEntry se;
        se.entry = p->getEntryInfo();
        int nsubentries = fillEntry(se, p->currentEntry());
        if (se.entry.size < 0) {
            // read entire stream to determine it's size
            StreamBase<char> *es = p->currentEntry();
            const char* c;
            while (es->read(c, 1, 0) > 0) {}
            se.entry.size = es->getSize();
            if (se.entry.size < 0) se.entry.size = 0;
        }
        addEntry(e, se);
        if (nsubentries) {
            nentries += nsubentries;
            e.entry.type = (EntryInfo::Type)(e.entry.type|EntryInfo::Dir);
        }
    } while (p->nextEntry());
    free(streams);
    return nentries;
}
ArchiveReader::ArchiveReader() {
    maxsize = 1024*1024;
    p = new ArchiveReaderPrivate();
}
ArchiveReader::~ArchiveReader() {
    delete p;
}
int
ArchiveReader::stat(const std::string& url, jstreams::EntryInfo& e) {
    if (localStat(url, e) == 0) return 0;

    // check the cache (this assumes getDirEntries was already called)
    const ArchiveEntryCache::SubEntry *subentry = p->cache.findEntry(url);
    if (subentry) {
        e = subentry->entry;
        return 0;
    }
    // try reading the entries from the collection to which this file belongs
    getDirEntries(url);
    subentry = p->cache.findEntry(url);
    if (subentry) {
        e = subentry->entry;
        return 0;
    }
    return -1;
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
ArchiveReader::addStreamOpener(StreamOpener* opener) {
    p->openers.push_back(opener);
}
void
ArchiveReader::closeStream(jstreams::StreamBase<char>* s) {
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
    if (localStat(url, e) != 0) {
        return false;
    }
    return e.type & EntryInfo::File && e.type & EntryInfo::Dir;
}
int
ArchiveReader::localStat(const std::string& url, jstreams::EntryInfo& e) {
    // try with the supplied streamOpeners
    list<StreamOpener*>::const_iterator i;
    for (i = p->openers.begin(); i != p->openers.end(); ++i) {
        if ((*i)->stat(url, e) == 0) {
            if (!(e.type & EntryInfo::File)) {
                return 0;
            }
            // check if this file is in the cache
            map<string, ArchiveEntryCache::RootSubEntry>::const_iterator se
                = p->cache.findRootEntry(url);
            if (se != p->cache.cache.end()) {
                if (se->second.entry.mtime == e.mtime) {
                    e.type = se->second.entry.type;
                    return 0;
                }
                p->cache.cache.erase(se->second.entry.filename);
            }

            // we statted, now we'd like to know if this file has subentries
            StreamBase<char>* s = (*i)->openStream(url);
            list<ArchiveReaderPrivate::StreamPtr> streams;
            SubStreamProvider* provider = p->getSubStreamProvider(s, streams);
            if (provider) {
                e.type = (EntryInfo::Type)(EntryInfo::Dir|EntryInfo::File);
                p->free(streams);

                ArchiveEntryCache::RootSubEntry rse;
                rse.indexed = false;
                rse.entry = e;
                p->cache.cache[url] = rse;
            }
            delete s;
            return 0;
        }
    }
    return -1;
}
DirLister
ArchiveReader::getDirEntries(const std::string& url) {
    std::vector<jstreams::EntryInfo> v;

    // find the entry in the cache
    const ArchiveEntryCache::SubEntry *subentry = p->cache.findEntry(url);

    // if this is a root entry that was not yet indexed, index it now
    if (subentry) {
        map<string, ArchiveEntryCache::RootSubEntry>::const_iterator se
            = p->cache.findRootEntry(url);
        if (se != p->cache.cache.end() && !se->second.indexed) {
            subentry = 0;
        }
    }
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
        localStat(name, e);
        ArchiveEntryCache::RootSubEntry se;
        se.indexed = true;
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
bool
ArchiveReader::canHandle(const std::string& url) {
    vector<size_t> partpos;
    size_t p = url.rfind('/');
    EntryInfo e;
    int r = localStat(url, e);
    while (p != string::npos && p != 0 && r == -1) {
        r = localStat(url.substr(0, p), e);
        partpos.push_back(p+1);
        p = url.rfind('/', p-1);
    }
    return r == 0 && e.type & EntryInfo::File && e.type & EntryInfo::Dir;
}
