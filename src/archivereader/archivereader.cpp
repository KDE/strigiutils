#include "archivereader.h"
#include "substreamprovider.h"
#include "tarinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include <vector>
using namespace jstreams;
using namespace std;

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
        printf("%s\n", url.substr(0, p).c_str());
        partpos.push_back(p);
        p = url.rfind('/', p-1);
    }
    if (!stream) {
        return 0;
    }

    // open the substreams until have opened the complete path
    SubStreamProvider* provider;
    StreamBase<char>* substream = stream;
    vector<size_t>::iterator i = partpos.begin();
    for (i++; i != partpos.end(); ++i) {
        provider = getSubStreamProvider(substream);
        if (provider == 0) {
            return 0;
        }
        string name = url.substr(*i);
        do {
            substream = provider->nextEntry();
            if (substream && provider->getEntryInfo().type == EntryInfo::File) {
                // check that the filename matches at least one entry
            }
        } while(substream);
    }
    return substream;
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
ArchiveReader::getSubStreamProvider(StreamBase<char>* input) {
    return new TarInputStream(new BZ2InputStream(input));
}
