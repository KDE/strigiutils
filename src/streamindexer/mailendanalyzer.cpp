#include "mailendanalyzer.h"
#include "mailinputstream.h"
#include "streamindexer.h"
#include "indexwriter.h"
using namespace jstreams;

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
}
char
MailEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* i) {
    MailInputStream mail(in);
    InputStream *s = mail.nextEntry();
    if (mail.getStatus() == jstreams::Error) {
        fprintf(stderr, "error reading mail: %i %s\n", mail.getStatus(),
            mail.getError());
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
        // maybe use the date of sending the mail here
        indexer->analyze(file, i->getMTime(), s, depth);
        s = mail.nextEntry();
        n++;
    }
    if (mail.getStatus() == jstreams::Error) {
        error = mail.getError();
        return -1;
    } else {
        error.resize(0);
    }
    return 0;
}

