#include "mailendanalyzer.h"
#include "mailinputstream.h"
#include "streamindexer.h"
using namespace jstreams;

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
}
char
MailEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable*) {
    MailInputStream mail(in);
    InputStream *s = mail.nextEntry();
    if (mail.getStatus() == jstreams::Error) {
        printf("error reading mail: %i %s\n", mail.getStatus(), mail.getError());
        exit(1);
    }
    int n = 1;
    while (s) {
        std::string file = filename+"/";
        if (mail.getEntryInfo().filename.length() == 0) {
            file += (char)(n+'1');
        } else {
            file += mail.getEntryInfo().filename;
        }
        indexer->analyze(file, s, depth);
        s = mail.nextEntry();
        n++;
    }
    if (mail.getStatus() == jstreams::Error) {
//        printf("Error: %s\n", tar.getError());
    } else {
//        printf("finished ok\n");
    }
    return mail.getStatus();
}

