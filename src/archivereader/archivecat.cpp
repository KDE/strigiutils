#include "jstreamsconfig.h"
#include "archivereader.h"
#include "fileinputstream.h"
#include "substreamprovider.h"
using namespace jstreams;
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>

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
            printf("read %lli bytes.\n", total);
            if (s->getStatus() == Error) {
                printf("Error: %s\n", s->getError());
            }
            reader.closeStream(s);
        }
    }
    return 0;
}

