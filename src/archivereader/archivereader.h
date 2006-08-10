#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include <list>
#include <string>

namespace jstreams {
    template <class T> class StreamBase;
    class SubStreamProvider;
}

class StreamOpener {
public:
    virtual ~StreamOpener() {}
    virtual jstreams::StreamBase<char>* openStream(const std::string& url) = 0;
};

class ArchiveReader : public StreamOpener {
private:
    std::list<StreamOpener*> openers;
    jstreams::StreamBase<char>* open(const std::string& url);
    jstreams::SubStreamProvider* getSubStreamProvider(
        jstreams::StreamBase<char>*);
public:
    ArchiveReader();
    ~ArchiveReader();
    jstreams::StreamBase<char>* openStream(const std::string& url);
    void addStreamOpener(StreamOpener* opener) {
        openers.push_back(opener);
    }
};

#endif
