#include "tarinputstream.h"
#include "subinputstream.h"
#include <cstring>

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
    output = new SubInputStream(input, entrySize);
    return output;
}
void
TarInputStream::readHeader(char *hb) {
    char *bptr;
    int toread;
    const char *begin;
    size_t nread;

    // read the first 500 characters
    toread = 512;
    bptr = hb;
    while (toread) {
        char r = input->read(begin, nread, toread);
        if (r) {
            status = -2;
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

    size_t len = 0;
    while (len < 108 && hb[len]) {
        len++;
    }
    if (len > 107) { // this is 107 because 100 + 7 (for the adjecent field)
                     // which may be merged by not having a proper '\0' at 100
        status = -2;
        error = "Error reading header: file name is too long.";
        return;
    }
    if (len == 0) {
        // ready
        status = -1;
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
    entrySize = readSize(hb);
    if (status) return;

    numPaddingBytes = 512 - entrySize%512;
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
size_t
TarInputStream::readSize(char *b) {
    size_t size;
    int r = sscanf(b+124, "%o", &size);
    if (r != 1) {
        status = -2;
        error = "Error reading header: size is not a valid integer.";
        return 0;
    }
    return size;
}
void
TarInputStream::readLongLink(char *b) {
    size_t toread = readSize(b);
    size_t left = toread%512;
    if (left) {
        left = 512 - left;
    }
    const char *begin;
    size_t nread;
    if (status) return;
    while (toread) {
        char r = input->read(begin, nread, toread);
        if (r) {
            status = -2;
            error = "Error reading LongLink: ";
            if (r == -2) {
                error += input->getError();
            } else {
                error += " premature end of file.";
            }
            return;
        }
        toread -= nread;
        entryinfo.filename.append(begin, nread);
    }
    char r = input->skip(left);
    if (r) {
        status = -2;
        error = "Error reading LongLink: ";
        if (r == -2) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
    }
}
