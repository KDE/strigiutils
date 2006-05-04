#include "base64inputstream.h"
using namespace jstreams;

/* code is based on public domain code at
   http://www.tug.org/ftp/vm/base64-decode.c
*/

const unsigned char Base64InputStream::alphabet[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
bool Base64InputStream::inalphabet[256];
unsigned char Base64InputStream::decoder[133];

Base64InputStream::Base64InputStream(StreamBase<char>* i) :input(i) {
    // initialize the translation arrays
    for (int i=64; i<256; ++i) {
        inalphabet[i] = 0;
    }
    for (int i=0; i<64; ++i) {
	inalphabet[alphabet[i]] = true;
	decoder[alphabet[i]] = i;
    }

    nleft = 0;
    char_count = 0;
    bits = 0;
    bitstodo = 0;
    pos = pend = 0;
}
bool
Base64InputStream::moreData() {
    if (pos == pend) {
        int32_t nread = input->read(pos, 1, 0);
        if (nread < -1) {
            status = Error;
            error = input->getError();
            return false;
        }
        if (nread <= 0) {
            status = Eof;
            return false;
        }
        pend = pos + nread;
    }
    return true;
}
int32_t
Base64InputStream::fillBuffer(char* start, int32_t space) {
    if (input == 0) return -1;
    // handle the  bytes that were left over from the last call
    //printf("%i space-\n", space);
    if (bitstodo) {
        //printf("-\n");
        switch (bitstodo) {
        case 3:
            *start = bits >> 16;
            break;
        case 2:
            *start = (bits >> 8) & 0xff;
            break;
        case 1:
            *start = bits & 0xff;
            bits = 0;
            char_count = 0;
            break;
        }
        bitstodo--;
        return 1;
    }
    const char* end = start + space;
    char* p = start;
    int32_t nwritten = 0;
    while (moreData()) {
        unsigned char c = *pos++;
        // = signals the end of the encoded block
        if (c == '=') {
            input = 0;
            break;
        }
        // we ignore characters that do not fit
        if (!inalphabet[c]) {
            continue;
        }
        bits += decoder[c];
        char_count++;
        if (char_count == 4) {
            if (p >= end) {
                bitstodo = 3;
                break;
            }
            *p++ = bits >> 16;
            //printf("%c\n", p[-1]);
            if (p >= end) {
                bitstodo = 2;
                nwritten++;
                break;
            }
            *p++ = (bits >> 8) & 0xff;
            //printf("%c\n", p[-1]);
            if (p >= end) {
                bitstodo = 1;
                nwritten += 2;
                break;
            }
            *p++ = bits & 0xff;
            //printf("%c\n", p[-1]);
            bits = 0;
            char_count = 0;
            nwritten += 3;
        } else {
            bits <<= 6;
        }
    }
    //printf(".");
    if (nwritten == 0 && status == Eof) {
        printf("EOF\n");
        nwritten = -1;
    }
    return nwritten;
}
