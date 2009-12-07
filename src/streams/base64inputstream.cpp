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
#include "base64inputstream.h"

#include <cstring>

using namespace std;
using namespace Strigi;

/* code is based on public domain code at
   http://www.tug.org/ftp/vm/base64-decode.c
*/

class Base64InputStream::Private {
public:
    Base64InputStream* const p;
    InputStream* input;
    const char* pos, * pend;
    int32_t bits;
    int32_t nleft;
    char bytestodo;
    char char_count;

    Private(Base64InputStream* p, InputStream* i);
    bool moreData();
    int32_t fillBuffer(char* start, int32_t space);
};


const unsigned char alphabet[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
bool inalphabet[256];
unsigned char decoder[133];
bool initializedAlphabet = false;
void initialize();
string decode(const char* in, string::size_type length);


void
initialize() {
    if (!initializedAlphabet) {
        initializedAlphabet = true;
        // initialize the translation arrays
        for (int i=64; i<256; ++i) {
            inalphabet[i] = 0;
        }
        for (unsigned char i=0; i<64; ++i) {
            inalphabet[alphabet[i]] = true;
            decoder[alphabet[i]] = i;
        }
    }
}

Base64InputStream::Base64InputStream(InputStream* i) 
    :p(new Private(this, i)) 
{
}

Base64InputStream::~Base64InputStream() {
    delete p;
}
Base64InputStream::Private::Private(Base64InputStream* bi, InputStream* i)
        :p(bi), input(i) {
    initialize();
    nleft = 0;
    char_count = 0;
    bits = 0;
    bytestodo = 0;
    pos = pend = 0;
}
bool
Base64InputStream::Private::moreData() {
    if (pos == pend) {
        int32_t nread = input->read(pos, 1, 0);
        if (nread < -1) {
            p->m_status = Error;
            p->m_error = input->error();
            input = 0;
            return false;
        }
        if (nread <= 0) {
            input = 0;
            return false;
        }
        pend = pos + nread;
    }
    return true;
}
int32_t
Base64InputStream::fillBuffer(char* start, int32_t space) {
    return p->fillBuffer(start, space);
}
int32_t
Base64InputStream::Private::fillBuffer(char* start, int32_t space) {
    if (input == 0 && bytestodo == 0) return -1;
    // handle the  bytes that were left over from the last call
    if (bytestodo) {
        switch (bytestodo) {
        case 3:
            *start = (char)(bits >> 16);
            break;
        case 2:
            *start = (char)((bits >> 8) & 0xff);
            break;
        case 1:
            *start = (char)(bits & 0xff);
            bits = 0;
            char_count = 0;
            break;
        }
        bytestodo--;
        return 1;
    }
    const char* end = start + space;
    char* p = start;
    int32_t nwritten = 0;
    while (moreData()) {
        unsigned char c = *pos++;
        // = signals the end of the encoded block
        if (c == '=') {
            if (char_count == 2) {
                bytestodo = 1;
                bits >>= 10;
            } else if (char_count == 3) {
                bytestodo = 2;
                bits >>= 8;
            }
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
                bytestodo = 3;
                break;
            }
            *p++ = (char)(bits >> 16);
            if (p >= end) {
                bytestodo = 2;
                nwritten++;
                break;
            }
            *p++ = (char)((bits >> 8) & 0xff);
            if (p >= end) {
                bytestodo = 1;
                nwritten += 2;
                break;
            }
            *p++ = (char)(bits & 0xff);
            bits = 0;
            char_count = 0;
            nwritten += 3;
        } else {
            bits <<= 6;
        }
    }
    if (nwritten == 0 && input == 0 && bytestodo == 0) {
//        printf("EOF\n");
        nwritten = -1;
    }
    return nwritten;
}
string
Base64InputStream::decode(const char* in, string::size_type length) {
    initialize();
    string d;
    if (length%4) return d;
    initialize();
    string::size_type words = length/4;
    d.reserve(words*3);
    const unsigned char* c = (const unsigned char*)in;
    const unsigned char* e = c + length;
    if (in[length-1] == '=') {
        e -= 4;
    }
    char k, l, b[3];
    for (; c < e; c += 4) {
         if (inalphabet[c[0]]  && inalphabet[c[1]] && inalphabet[c[2]]
             && inalphabet[c[3]]) {
            k = decoder[c[1]];
            l = decoder[c[2]];
            b[0] = (char)((decoder[c[0]] << 2) + (k >> 4));
            b[1] = (char)((k             << 4) + (l >> 2));
            b[2] = (char)((l             << 6) + (decoder[c[3]]));
            d.append(b, 3);
         } else {
             return string();
         }
    }
    if (in[length-2] == '=') {
        if (inalphabet[c[0]]  && inalphabet[c[1]]) {
            b[0] = (char)((decoder[c[0]] << 2)+((decoder[c[1]] >> 4)));
            d.append(b, 1);
        } else {
            return string();
        }
    } else if (in[length-1] == '=') {
        if (inalphabet[c[0]]  && inalphabet[c[1]] && inalphabet[c[2]]) {
            k = decoder[c[1]];
            b[0] = (char)((decoder[c[0]] << 2) + (k >> 4));
            b[1] = (char)((k             << 4) + (decoder[c[2]] >> 2));
            d.append(b, 2);
        } else {
            return string();
        }
    }
    return d;
}
