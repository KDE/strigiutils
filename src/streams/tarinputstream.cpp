#include "tarinputstream.h"
#include "subinputstream.h"
#include <cstring>
using namespace jstreams;

TarInputStream::TarInputStream(InputStream *input)
        : SubStreamProvider(input) {
    output = 0;
}

TarInputStream::~TarInputStream() {
    if (output) {
        delete output;
    }
}
SubInputStream*
TarInputStream::nextEntry() {
    if (status) return 0;
    if (output) {
        output->skipToEnd();
        input->skip(numPaddingBytes);
        delete output;
        output = 0;
    }
    parseHeader();
    if (status) return 0;
    output = new SubInputStream(input, entryinfo.size);
    return output;
}
void
TarInputStream::readHeader(char *hb) {
    char *bptr;
    int toread;
    const char *begin;
    int32_t nread;

    // read the first 500 characters
    toread = 512;
    bptr = hb;
    while (toread) {
        StreamStatus r = input->read(begin, nread, toread);
        if (r) {
            status = Error;
            error = "Error reading header: " + input->getError();
            return;
        }
        memcpy(bptr, begin, nread);
        bptr += nread;
        toread -= nread;
    }
}
void
TarInputStream::parseHeader() {
    char hb[512];
    readHeader(hb);
    if (status) return;

    int32_t len = 0;
    while (len < 108 && hb[len]) {
        len++;
    }
    if (len > 107) { // this is 107 because 100 + 7 (for the adjecent field)
                     // which may be merged by not having a proper '\0' at 100
        status = Error;
        error = "Error reading header: file name is too long.";
        return;
    }
    if (len == 0) {
        // ready
        status = Eof;
        return;
    }
    if (len > 100) len = 100;
    entryinfo.filename.resize(0);
    entryinfo.filename.append(hb, len);
    if (entryinfo.filename == "././@LongLink") {
        entryinfo.filename.resize(0);
        readLongLink(hb);
        if (status) return;
        readHeader(hb);
    }

    // read the file size which is in octal format
    entryinfo.size = readOctalField(hb, 124);
    if (status) return;
    entryinfo.mtime = readOctalField(hb, 136);
    if (status) return;

    numPaddingBytes = 512 - entryinfo.size%512;
    if (numPaddingBytes == 512) {
        numPaddingBytes = 0;
    }

    len = entryinfo.filename.length();
    if (entryinfo.filename[len-1] == '/') {
        entryinfo.filename.resize(len-1);
        entryinfo.type = EntryInfo::Dir;
    } else {
        entryinfo.type = EntryInfo::File;
    }
    //printf("%s\n", entryfilename.c_str());
}
int32_t
TarInputStream::readOctalField(char *b, int32_t offset) {
    int32_t val;
    int r = sscanf(b+offset, "%o", &val);
    if (r != 1) {
        status = Error;
        error = "Error reading header: octal field is not a valid integer.";
        return 0;
    }
    return val;
}
void
TarInputStream::readLongLink(char *b) {
    int32_t toread = readOctalField(b, 124);
    int32_t left = toread%512;
    if (left) {
        left = 512 - left;
    }
    const char *begin;
    int32_t nread;
    if (status) return;
    while (toread) {
        StreamStatus r = input->read(begin, nread, toread);
        if (r) {
            status = Error;
            error = "Error reading LongLink: ";
            if (r == Error) {
                error += input->getError();
            } else {
                error += " premature end of file.";
            }
            return;
        }
        toread -= nread;
        entryinfo.filename.append(begin, nread);
    }
    StreamStatus r = input->skip(left);
    if (r) {
        status = Error;
        error = "Error reading LongLink: ";
        if (r == Error) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
    }
}
