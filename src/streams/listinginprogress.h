#ifndef LISTINGINPROGRESS_H
#define LISTINGINPROGRESS_H

#include "archiveentrycache.h"
#include "archivereader.h"
#include <list>
#include <vector>

typedef std::map<bool (*)(const char*, int32_t),
    Strigi::SubStreamProvider* (*)(Strigi::InputStream*)> Subs;

/** Convenience class that holds a pointer to either a stream or a
 *  SubStreamProvider. It ensures proper destruction of the objects pointed
 *  to.
 **/
class StreamPtr {
private:
    Strigi::InputStream* stream;
    Strigi::SubStreamProvider* provider;
public:
    StreamPtr() :stream(0), provider(0) {}
    StreamPtr(Strigi::InputStream* s) :stream(s), provider(0) {}
    StreamPtr(Strigi::SubStreamProvider* p) :stream(0), provider(p) {}
    void free() {
        delete stream;
        delete provider;
    }
};

Strigi::SubStreamProvider*
subStreamProvider(const Subs& subs, Strigi::InputStream* input,
         std::list<StreamPtr>& streams);
void free(std::list<StreamPtr>& l);

class StackEntry {
public:
    std::list<StreamPtr> streams; // stream stack that lead to this SubStreamProvider
    Strigi::SubStreamProvider* p;
    ArchiveEntryCache::SubEntry* entry; // the current entry
};

/*
 * This class contains a stack of SubStreamProviders and a growing cache
 * entry. It serves as a common entrypoint for requests to urls below a
 * common prefix that points to a physical file.
 * If a request is given to ArchiveReader for a path that is not in the
 * cache yet and that path could logically be part of a physical file,
 * ArchiveReader will look if there is a ListingInProgress. If there is,
 * the request is sent to the ListingInProgress. It may have the required
 * entry already in cache. If not, it should read from the stack of
 * SubStreamProviders until it can service the incoming request.
 */
class ListingInProgress {
private:
    const Subs subs;
    Strigi::InputStream* stream;
    std::vector<StackEntry> stack;
    int currentdepth;
    int refcount;
public:
    ArchiveEntryCache::RootSubEntry* root;
    const std::string url;

    ListingInProgress(const Subs& sbs, const Strigi::EntryInfo& entry,
        const std::string& u, Strigi::InputStream* s);
    ~ListingInProgress();
    bool isDone() const;
    void fillEntry(Strigi::InputStream* s);
    int nextEntry(int depth);
    bool nextEntry();
    const ArchiveEntryCache::SubEntry* nextEntry(const std::string& url);
    bool nextEntry(const ArchiveEntryCache::SubEntry* entry);
    void ref() { refcount++; }
    bool unref() { return --refcount == 0; }
};


#endif
