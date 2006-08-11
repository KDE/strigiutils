#include "archivereader.h"
#include "substreamprovider.h"
#include "tarinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include <vector>
using namespace jstreams;
using namespace std;

void
ArchiveReader::StreamPtr::free() {
    if (stream) delete stream;
    if (provider) delete provider;
}
ArchiveReader::ArchiveReader() {
}
ArchiveReader::~ArchiveReader() {
}
StreamBase<char>*
ArchiveReader::openStream(const string& url) {
    StreamBase<char>* stream = open(url);
    if (stream) return stream;

    // cull the url until a stream can be opened
    vector<size_t> partpos;
    size_t p = url.rfind('/');
    while (p != string::npos && !stream) {
        stream = open(url.substr(0, p));
        partpos.push_back(p+1);
        p = url.rfind('/', p-1);
    }
    if (!stream) {
        return 0;
    }

    // open the substreams until have opened the complete path
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
        do {
            substream = provider->nextEntry();
            const EntryInfo& e = provider->getEntryInfo();
            // check that the filename matches at least one entry
            if (substream && e.type == EntryInfo::File
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
                    return substream;
                }
                --i;
            }
        } while(substream && !nextstream);
    }
    if (substream) {
        openstreams[substream] = streams;
    } else {
        free(streams);
    }
    return substream;
}
void
ArchiveReader::closeStream(jstreams::StreamBase<char>* s) {
    openstreamsType::iterator i = openstreams.find(s);
    if (i == openstreams.end()) return;
    free(i->second);
    openstreams.erase(i);
}
void
ArchiveReader::free(list<StreamPtr>& l) {
    list<StreamPtr>::iterator i;
    for (i=l.begin(); i!=l.end(); ++i) {
        i->free();
    }
}
StreamBase<char>*
ArchiveReader::open(const string& url) {
    StreamBase<char>* stream = 0;
    list<StreamOpener*>::const_iterator i;
    for (i = openers.begin(); i != openers.end() && stream == 0; ++i) {
        stream = (*i)->openStream(url);
    }
    return stream;
}
SubStreamProvider*
ArchiveReader::getSubStreamProvider(StreamBase<char>* input,
        list<ArchiveReader::StreamPtr>& streams) {
    if (input == 0) return 0;
    StreamBase<char>* s = new BZ2InputStream(input);
    if (s->getStatus() == Ok) {
        streams.push_back(s);
    } else {
        printf("no bz2\n", input);
        delete s;
        input->reset(0);
        s = new GZipInputStream(input);
        if (s->getStatus() != Ok) {
            printf("no gz\n", input);
            delete s;
            input->reset(0);
            s = input;
        } else {
            streams.push_back(s);
        }
    }
    SubStreamProvider* ss = new TarInputStream(s);
    if (ss->getStatus() == Ok) {
        streams.push_back(ss);
    } else {
        delete ss;
        ss = 0;
    }
    return ss;
}
int
ArchiveReader::stat(const std::string& url, jstreams::EntryInfo& e) {
    return -1;
}
