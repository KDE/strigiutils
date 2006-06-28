#include "mimetypethroughanalyzer.h"
#include "indexwriter.h"
using namespace std;

MimeTypeThroughAnalyzer::MimeTypeThroughAnalyzer() {
    magic = magic_open(MAGIC_MIME);
    magic_load(magic, 0);
}
MimeTypeThroughAnalyzer::~MimeTypeThroughAnalyzer() {
    magic_close(magic);
}
jstreams::InputStream *
MimeTypeThroughAnalyzer::connectInputStream(jstreams::InputStream *in) {
    // determine the mimetype
    const char* mime;
    int64_t pos = in->getPosition();
    int32_t n = in->read(mime, 1, 0);
    in->reset(pos);
    if (n >= 0) {
        mime = magic_buffer(magic, mime, n);
        if (mime == 0) return in;
        const char* slash = strchr(mime, '/');
        if (slash == 0) return in;
        int l = strcspn(mime, " \t;\\");
        if (l < slash-mime) return in;
        idx->setMimeType(string(mime, l));
        const char* charset = strstr(mime+l, "charset=");
        if (charset == 0) return in;
        charset += 8;
        idx->setEncoding(charset);
    }
    return in;
}
