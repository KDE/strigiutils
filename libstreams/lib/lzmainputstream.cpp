/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/lzmainputstream.h>
extern "C" {
  #include "lzma/LzmaDec.h"
}
#include <strigi/textutils.h>
#include <iostream>
#include <sstream>

using namespace Strigi;
using namespace std;

class LZMAInputStream::Private {
public:
    LZMAInputStream* const p;
    CLzmaProps props;
    CLzmaDec state;
    InputStream *input;
    const char* next_in;
    int32_t avail_in;
    int64_t bytesDecompressed;

    Private(LZMAInputStream* p, InputStream* i);
    ~Private();
    void readFromStream();
    int32_t fillBuffer(char* start, int32_t space);
};
bool
LZMAInputStream::checkHeader(const char* data, int32_t datasize) {
    if (datasize < LZMA_PROPS_SIZE + 8) return false;

    // lzma does not have magic bytes, but it has a function for parsing
    // the header which contains the stream properties
    // nevertheless some use '5d 00' as the magic bytes
    if (data[0] != 0x5d || data[1] != 0x00) return false;

    // by limiting the range of sizes and doing a sanity check on the
    // size of the dictionary, the number of files that are regarded as valid
    // can be limited
    CLzmaProps props;
    SRes res = LzmaProps_Decode(&props, (const Byte*)data, LZMA_PROPS_SIZE);
    int64_t size = readLittleEndianInt64(data + LZMA_PROPS_SIZE);
    return res == SZ_OK && props.dicSize <= 0x2000000 && (size == -1 ||
        (props.dicSize < size && size < 1099511627776LL)); // only support < 1Tb 
}
LZMAInputStream::LZMAInputStream(InputStream* input)
    :p(new Private(this, input)){
}
static void *SzAlloc(void *p, size_t size) { p = p; return malloc(size); }
static void SzFree(void *p, void *address) { p = p; free(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };
LZMAInputStream::Private::Private(LZMAInputStream* bis, InputStream* i)
        :p(bis), input(i) {

    // check the header and read the size of the uncompressed stream
    SRes res = SZ_ERROR_UNSUPPORTED;
    LzmaDec_Construct(&state);
    const char* data;
    int32_t nread;
    const int32_t headersize = LZMA_PROPS_SIZE+8;
    nread = input->read(data, headersize, headersize);
    if (nread == headersize && checkHeader(data, headersize)) {
        // allocate the decompressor
        res = LzmaDec_Allocate(&state, (const Byte*)data, LZMA_PROPS_SIZE,
            &g_Alloc);
        p->m_size = readLittleEndianInt64(data + LZMA_PROPS_SIZE);
    }
    if (res != SZ_OK || p->m_size < -1) {
        p->m_error = "LZMA header is not supported.";
        p->m_status = Error;
        return;
    }
    LzmaDec_Init(&state);
    bytesDecompressed = 0;

    // signal that we need to read into the buffer
    avail_in = 0;

    // set the minimum size for the output buffer
    p->setMinBufSize(262144);
}
LZMAInputStream::~LZMAInputStream() {
    delete p;
}
LZMAInputStream::Private::~Private() {
    LzmaDec_Free(&state, &g_Alloc);
}
void
LZMAInputStream::Private::readFromStream() {
    // read data from the input stream
    avail_in = input->read(next_in, 1, 0);
    if (avail_in <= -1) {
        p->m_status = Error;
        p->m_error = input->error();
    } else if (avail_in < 1) {
        p->m_status = Error;
        p->m_error = "unexpected end of stream";
    }
}
int32_t
LZMAInputStream::fillBuffer(char* start, int32_t space) {
    if (m_status != Ok) return -1;
    if (m_size == p->bytesDecompressed) {
        return -1;
    }
    return p->fillBuffer(start, space);
}
int32_t
LZMAInputStream::Private::fillBuffer(char* start, int32_t space) {
    // make sure there is data to decompress
    if (avail_in == 0) {
        readFromStream();
        if (p->m_status != Ok) {
            // no data was read
            return -1;
        }
    }
    // decompress
    SizeT outProcessed = space;
    SizeT inProcessed = avail_in;
    ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
    int64_t bytesTodo = p->m_size - bytesDecompressed;
    if (p->m_size != -1 && outProcessed > (SizeT)bytesTodo) {
        outProcessed = (SizeT)bytesTodo;
        finishMode = LZMA_FINISH_END;
    }
    ELzmaStatus status;
    SRes res = LzmaDec_DecodeToBuf(&state, (Byte*)start, &outProcessed,
        (const Byte*)next_in, &inProcessed, finishMode, &status);
    avail_in -= (int32_t)inProcessed;
    next_in += (int32_t)inProcessed;
    bytesDecompressed += outProcessed;
    if (res != SZ_OK) {
        ostringstream str;
        str << "error decompressing dicsize: " << props.dicSize
            << " size: " << p->m_size
            << " decompressed: " << bytesDecompressed;
        p->m_error = str.str();
        p->m_status = Error;
        return -1;
    }
    if (inProcessed == 0 && outProcessed == 0) {
        if (p->m_size != -1 || status != LZMA_STATUS_FINISHED_WITH_MARK) {
            p->m_error = "unexpected end";
            p->m_status = Error;
            return -1;
        }
        p->m_size = bytesDecompressed;
    } else if (status == LZMA_STATUS_FINISHED_WITH_MARK) {
        p->m_size = bytesDecompressed;
    }
    return (int32_t)outProcessed;
}
