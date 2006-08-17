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
    printf("name:'%s'\n", e.filename.c_str());

    return 0;
}
int
main(int argc, char** argv) {
    EntryInfo e;
    FileStreamOpener opener;

    ArchiveReader reader;
    reader.addStreamOpener(&opener);

    for (int i=1; i<argc; ++i) {
        DirLister dl = reader.getDirEntries(argv[i]);
        while (dl.nextEntry(e)) {
            printf("%s\n", e.filename.c_str());
        }
/*        if (reader.stat(argv[i], e) != -1) {
            printf("name: %s\n", e.filename.c_str());
            printf("size: %i\n", e.size);
            printf("type: %i\n", e.type);
            if (e.type == EntryInfo::Dir) {
                printf("get dir\n");
                reader.getDirEntries(argv[i]);
            } else {
                StreamBase<char>* s = reader.openStream(argv[i]);
                reader.closeStream(s);
            }
        } else {
            fprintf(stderr, "Could not read '%s'\n", argv[i]);
        }*/
    }
    return 0;
}

