/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <strigi/strigiconfig.h>
#include "fileinputstream.h"
#include "stringstream.h"
#include "gzipinputstream.h"
#include "kmpsearcher.h"
#include <ctype.h>
extern "C" {
    #include <magic.h>
}

using namespace Strigi;

/* functions missing from the stream:
   - is the next value a certain string?
   - skip x character, throw exception on failure
   - advance one character
   - ensure the pointer points to the next 4 characters
*/

bool
isPdfWhitespace(char c) {
    return c==0x00||c==0x09||c==0x0A||c==0x0C||c==0x0D||c==0x20;
}
bool
isPdfDelimiter(char c) {
    return c=='('||c==')'||c=='<'||c=='>'||c=='['||c==']'||c=='{'||c=='}'
        ||c=='/'||c=='%';
}


/*
 * parser for files like this one:
 * http://partners.adobe.com/public/developer/en/pdf/PDFReference16.pdf
 */
KmpSearcher stream;
KmpSearcher endstream;
KmpSearcher length;
KmpSearcher filter;
KmpSearcher type;
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
    if (file == 0) return;
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
extractText(const char* start, const char* end) {
    printf("%i\n", count);
    const char* p=start;
    bool on1 = false;
    bool on2 = false;
    bool space = false;
    //bool escape = false;
    while (p != end && isascii(*p)) {
        char c = *p;
        space = c == ' ';
        switch (c) {
        case ('['): on1 =  true; break;
        case (']'): on1 = false; if (!space) printf(" "); break;
        case ('('): on2 =  true; break;
        case (')'): on2 = false; break;
        default: if (on1 && on2) printf("%c", c);
        }
        p++;

    }
    printf("\n");
}
void
extractText2(const char* start, const char* end) {
    printf("%i\n", count);
    const char* p=start;
    bool on = false;
    //bool space = false;
    bool escape = false;
    while (p != end && isascii(*p)) {
        char c = *p;
        if (on) {
            if (escape) {
                printf("%c", c);
                escape = false;
            } else if (c == ')') {
                on = false;
            } else {
                printf("%c", c);
            }
        } else if (c == '(') {
            on = true;
        }
        p++;
    }
}
void
analyze(StreamBase<char>* s) {
    const char* c;
    int32_t n = s->read(c, 1024, 0);
    while (n >= 0 && s->status() == Ok) {
        n = s->read(c, 2*n, 0);
    }
    if (n <= 0) {
        return;
    }
//    saveFile(c, c+n);
//    printf("%i\n", n);
//    if (n > 7 && strncmp(c, "/GS2 gs", 7) == 0) extractText2(c, c+n);
    analyzeMime(c, c+n);
}
const char*
findDictStart(const char* start, const char* end) {
    const char* d = end - 7;
    int count = 1;
    // move to the last '>' of the dict
    while (d > start && *d != '>') --d;
    --d;
    while (count && d > start && isascii(*d)) {
        switch (*d) {
        case '>':
            count++;
            --d;
            break;
        case '<':
            count--;
        default:
            --d;
        }
    }
    d++;
    return d;
}
const char*
nextStream(const char*start, const char* end) {
    // find the start of the next stream
    const char* s = stream.search(start, end-start);
    if (!s || end-s < 8 || s[-1] == 'd') return s;
    if (s[6] == '\r') s++;
    if (s[6] != '\n') return s;
    s += 7;

    // find the start of the preceding dictionary
    const char* d = findDictStart(start, s);
    d++;
    printf("%i %.*s\n", count, (int)(s-d), d);

    // find the end of the stream
    int32_t l = -1;
    const char* v = length.search(d, s-d);
    char* ve;
    if (v) {
        v += length.queryLength();
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
    StringInputStream ss(s, e-s, false);
    //printf("%.*s\n", s-d, d);

    // do we have a type?
    bool havetype = type.search(d, s-d)!=NULL;

    // is the FlatDecode filter applied ?
    v = filter.search(d, s-d);
    if (v) {
        v+= filter.queryLength();
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
        int32_t n = file.read(c, file.size(), 0);
        const char* end = c+n;
        const char* s = nextStream(c, end);
        while (s++) {
            s = nextStream(s, end);
        }
    }
    printf("%i\n", count);
    magic_close(magic);
    return 0;
}
