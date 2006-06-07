#include "mailendanalyzer.h"
#include "mailinputstream.h"
#include "streamindexer.h"
#include "indexwriter.h"
#include "textendanalyzer.h"
using namespace jstreams;

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
}
char
MailEndAnalyzer::analyze(std::string filename, InputStream *in,
        int depth, StreamIndexer *indexer, jstreams::Indexable* idx) {
    MailInputStream mail(in);
    InputStream *s = mail.nextEntry();
    if (s == 0 || mail.getStatus() == jstreams::Error) {
        fprintf(stderr, "error reading mail: %i %s\n", mail.getStatus(),
            mail.getError());
        exit(1);
    }
    idx->setField("title", mail.getSubject());
    idx->setField("contenttype", mail.getContentType());
    if (mail.getSubject().length() == 0) {
        printf("no subject for %s\n", filename.c_str());
    }
    TextEndAnalyzer tea;
    if (tea.analyze(filename, s, depth, indexer, idx) != 0) {
        error = "Error reading mail body.";
        return -1;
    }
    s = mail.nextEntry();
    int n = 1;
    while (s) {
        std::string file = filename+"/";
        if (mail.getEntryInfo().filename.length() == 0) {
            file += (char)(n+'1');
        } else {
            file += mail.getEntryInfo().filename;
        }
        // maybe use the date of sending the mail here
        indexer->analyze(file, idx->getMTime(), s, depth);
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

