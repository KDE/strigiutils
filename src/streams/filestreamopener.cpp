#include "archivereader.h"
#include "fileinputstream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace jstreams;
using namespace std;

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
    if (::stat(url.c_str(), &s) == -1) {
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
