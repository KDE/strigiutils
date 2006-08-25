#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include "substreamprovider.h"

namespace jstreams {
    template <class T> class StreamBase;
    class SubStreamProvider;
    class EntryInfo;
}

class DirLister {
private:
    int pos;
    std::vector<jstreams::EntryInfo> entries;
public:
    DirLister(const std::vector<jstreams::EntryInfo>& e)
        : pos(0), entries(e) {
    }
    bool nextEntry(jstreams::EntryInfo& e);
};

class StreamOpener {
public:
    virtual ~StreamOpener() {}
    virtual jstreams::StreamBase<char>* openStream(const std::string& url) = 0;
    virtual int stat(const std::string& url, jstreams::EntryInfo& e) = 0;
};

class ArchiveReader : public StreamOpener {
private:
    class ArchiveReaderPrivate;
    ArchiveReaderPrivate *p;
public:
    ArchiveReader();
    ~ArchiveReader();
    jstreams::StreamBase<char>* openStream(const std::string& url);
    void closeStream(jstreams::StreamBase<char>*);
    int stat(const std::string& url, jstreams::EntryInfo& e);
    void addStreamOpener(StreamOpener* opener);
    DirLister getDirEntries(const std::string& url);
    bool isArchive(const std::string& url);
};

#endif
