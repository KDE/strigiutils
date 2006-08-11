#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include <map>
#include <list>
#include <string>

namespace jstreams {
    template <class T> class StreamBase;
    class SubStreamProvider;
    class EntryInfo;
}

class StreamOpener {
public:
    virtual ~StreamOpener() {}
    virtual jstreams::StreamBase<char>* openStream(const std::string& url) = 0;
    virtual int stat(const std::string& url, jstreams::EntryInfo& e) = 0;
};

class ArchiveReader : public StreamOpener {
private:
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

    jstreams::StreamBase<char>* open(const std::string& url);
    jstreams::SubStreamProvider* getSubStreamProvider(
        jstreams::StreamBase<char>*, std::list<StreamPtr>& streams);
    static void free(std::list<StreamPtr>& l);
public:
    ArchiveReader();
    ~ArchiveReader();
    jstreams::StreamBase<char>* openStream(const std::string& url);
    void closeStream(jstreams::StreamBase<char>*);
    int stat(const std::string& url, jstreams::EntryInfo& e);
    void addStreamOpener(StreamOpener* opener) {
        openers.push_back(opener);
    }
};

#endif
