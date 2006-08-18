#include "fileinputstream.h"
#include "stringreader.h"
#include "gzipinputstream.h"
#include "kmpsearcher.h"
using namespace jstreams;
extern "C" {
    #include <magic.h>
}

/*
 * parser for files like this one:
 * http://partners.adobe.com/public/developer/en/pdf/PDFReference16.pdf
 */
KMPSearcher stream;
KMPSearcher endstream;
KMPSearcher length;
KMPSearcher filter;
KMPSearcher type;
int count;
magic_t magic;

bool
ispdfblank(char c) {
    return c==0||c==9||c==10||c==12||c==13||c==32;
}

void
saveFile(const char* start, const char* end, const char* suf=0) {
    char name[32];
    if (suf) {
        sprintf(name, "out/%i.%s", count, suf);
    } else {
        sprintf(name, "out/%i", count);
    }
    FILE* file = fopen(name, "wb");
    fwrite(start, 1, end-start, file);
    fclose(file);
}

void
analyzeMime(const char* start, const char* end) {
    const char* mime = magic_buffer(magic, start, end-start);
    if (mime == 0) return;
    if (strcmp(mime, "audio/mpg") == 0) {
        saveFile(start, end, "mp3");
    } else if (strncmp(mime, "text/plain", 10) == 0) {
        saveFile(start, end, "txt");
    } else if (strcmp(mime, "image/jpeg") == 0) {
        saveFile(start, end, "jpg");
    } else if (strcmp(mime, "image/tiff") == 0) {
        saveFile(start, end, "tiff");
    } else {//if (strcmp(mime, "application/octet-stream")) {
        saveFile(start, end);
//        printf("%s\n", mime);
    }
}
void
analyze(StreamBase<char>* s) {
    const char* c;
    int32_t n = s->read(c, 1024, 0);
    while (n >= 0 && s->getStatus() == Ok) {
        n = s->read(c, 2*n, 0);
    }
    if (n <= 0) {
        return;
    }
//    saveFile(c, c+n);
//    printf("%i\n", n);
    if (*c == '/') return;
    analyzeMime(c, c+n);
}

const char*
getStream(const char*start, const char* end) {
    // find the start of the next stream
    const char* s = stream.search(start, end-start);
    if (!s || end-s < 8 || s[-1] == 'd') return s;
    if (s[6] == '\r') s++;
    if (s[6] != '\n') return s;
    s += 7;

    // find the start of the preceding dictionary
    const char* d = s-7;
    while (d > start && isascii(*d) && *d != '<') --d;
    d++;

    // find the end of the stream
    int32_t l = -1;
    const char* v = length.search(d, s-d);
    char* ve;
    if (v) {
        v += length.getQueryLength();
        l = strtol(v, &ve, 10);
        if (v != ve) {
            v = ve;
            // check if there's another int, if so length is a reference
            strtol(v, &ve, 10);
            if (v != ve) l = -1;
        } else {
            l = -1;
        }
    }
    const char* e;
    if (l != -1) {
        v = e = s + l;
        if (v + 8 > end) return end;
        if (*v == 10) v++;
        if (*v == 13) v++;
        if (strncmp(v, "endstream", 9) != 0) {
            return v+8;
        }
    } else {
        e = endstream.search(s, end-s);
        if (!e) return e;
    }

    // now we have a stream from s to e
    StringReader<char> ss(s, e-s, false);
//    printf("%.*s\n", s-d, d);

    // do we have a type?
    bool havetype = type.search(d, s-d);

    // is the FlatDecode filter applied ?
    v = filter.search(d, s-d);
    if (v) {
        v+= filter.getQueryLength();
        while (ispdfblank(*v)) v++;
    }
    if (v && strncmp(v, "/FlateDecode", 12) == 0) {
        if (!havetype){
            GZipInputStream gzip(&ss, GZipInputStream::ZLIBFORMAT);
            analyze(&gzip);
        } else {
//            printf("have type\n");
        }
    } else {
        analyze(&ss);
    }

    count++;
    return e;
}

int
main(int argc, char** argv) {
    count = 0;
    stream.setQuery("stream");
    endstream.setQuery("endstream");
    length.setQuery("/Length ");
    filter.setQuery("/Filter");
    type.setQuery("/Type");

    magic = magic_open(MAGIC_MIME);
    magic_load(magic, 0);

    for (int i=1; i<argc; ++i) {
        FileInputStream file(argv[i]);
        const char* c;
        int32_t n = file.read(c, file.getSize(), 0);
        const char* end = c+n;
        const char* s = getStream(c, end);
        while (s++) {
            s = getStream(s, end);
        }
    }
    printf("%i\n", count);
    magic_close(magic);
    return 0;
}
