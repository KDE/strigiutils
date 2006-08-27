#include "archivereader.h"
#include "fileinputstream.h"
#include "substreamprovider.h"
using namespace jstreams;
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>

class FileStreamOpener : public StreamOpener {
public:
    ~FileStreamOpener() {}
    StreamBase<char>* openStream(const string& url);
    int stat(const string& url, EntryInfo& e);
};
StreamBase<char>*
FileStreamOpener::openStream(const string& url) {
    StreamBase<char>* stream = new FileInputStream(url.c_str());
    if (stream->getStatus() != Ok) {
        delete stream;
        stream = 0;
    }
    return stream;
}
int
FileStreamOpener::stat(const string& url, EntryInfo& e) {
    struct stat s;
    if (lstat(url.c_str(), &s) == -1) {
        return -1;
    }
    if (S_ISREG(s.st_mode)) {
        e.type = EntryInfo::File;
    } else if (S_ISDIR(s.st_mode)) {
        e.type = EntryInfo::Dir;
    } else {
        e.type = EntryInfo::Unknown;
    }
    e.size = s.st_size;
    e.mtime = s.st_mtime;
    size_t p = url.rfind('/');
    if (p == string::npos) {
        e.filename = url;
    } else {
        e.filename = url.substr(p+1);
    }

    return 0;
}
int
main(int argc, char** argv) {
    EntryInfo e;
    FileStreamOpener opener;

    ArchiveReader reader;
    reader.addStreamOpener(&opener);

    for (int i=1; i<argc; ++i) {
        EntryInfo e;
        if (reader.stat(argv[i], e) != 0) {
            printf("could not read %s\n", argv[i]);
            continue;
        }
        printf("file: %s\n", e.filename.c_str());
        DirLister dl = reader.getDirEntries(argv[i]);
        while (dl.nextEntry(e)) {
            printf("%s\n", e.filename.c_str());
        }
        StreamBase<char>* s = 0;
        if (e.type & EntryInfo::File) {
            s = reader.openStream(argv[i]);
        }
        if (s) {
            const char* c;
            int64_t total = 0;
            int32_t n = s->read(c, 1, 0);
            while (n > 0) {
                total += n;
                n = s->read(c, 1, 0);
            }
            printf("read %i bytes.\n", total);
            if (s->getStatus() == Error) {
                printf("Error: %s\n", s->getError());
            }
            reader.closeStream(s);
        }
    }
    return 0;
}

