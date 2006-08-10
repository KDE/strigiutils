#include "archivereader.h"
#include "fileinputstream.h"
using namespace jstreams;
using namespace std;

class FileStreamOpener : public StreamOpener {
public:
    ~FileStreamOpener() {}
    StreamBase<char>* openStream(const string& url);
};

StreamBase<char>*
FileStreamOpener::openStream(const string& url) {
    StreamBase<char>* stream = new FileInputStream(url.c_str());
    if (stream->getStatus() != Ok) {
        delete stream;
        stream = 0;
    }
    printf("oS %p\n", stream);
    return stream;
}

int
main(int argc, char** argv) {
    FileStreamOpener opener;
    ArchiveReader reader;
    reader.addStreamOpener(&opener);

    StreamBase<char>* s = reader.openStream(
        "/tmp/zlib-1.2.3.tar.bz2/zlib-1.2.3/README");
    return 0;
}

