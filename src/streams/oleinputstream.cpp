/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "oleinputstream.h"
#include "textutils.h"
#include "bufferedstream.h"
using namespace Strigi;
using namespace std;

class Strigi::OleEntryStream : public BufferedInputStream {
public:
    OleInputStream* const parent;
    int64_t done;
    int32_t blockoffset;
    int32_t blocksize;

    OleEntryStream(OleInputStream* f) :parent(f), blockoffset(0) {
        setMinBufSize(512);
        done = 0;
        blockoffset = 0;
        m_size = parent->currentStreamSize;
    }
    void reset() {
        resetBuffer();
        done = 0;
        blockoffset = 0;
        m_size = parent->currentStreamSize;
        blocksize = (m_size < 4096) ?64 : 512;
    }
    int32_t fillBuffer(char* start, int32_t space);
};
int32_t
OleEntryStream::fillBuffer(char* start, int32_t space) {
    if (done == m_size) return -1;

    int32_t n = space;
    int32_t avail = blocksize-blockoffset;
    if (avail > m_size-done) {
        avail = m_size-done;
    }
    if (n > avail) {
        n = avail;
    }
    const char* d;
    if (blocksize == 64) {
        d = parent->getCurrentSmallBlock();
        if (d == 0) {
            m_status = Error;
            fprintf(stderr, "error in small blocks\n");
	    return -1;
        }
    } else {
        d = parent->data+(1+parent->currentDataBlock)*512;
    }
    memcpy(start, d+blockoffset, n);
    done += n;
    blockoffset += n;
    if (blockoffset == blocksize) {
        if (blocksize == 64) {
            parent->currentDataBlock
                = parent->nextSmallBlock(parent->currentDataBlock);
        } else {
            parent->currentDataBlock
                = parent->nextBlock(parent->currentDataBlock);
        }
        blockoffset = 0;
        if (parent->currentDataBlock < 0) {
            if (parent->currentDataBlock != -2 || done != m_size) {
//                fprintf(stderr, "error: %i\n", parent->currentDataBlock);
                m_status = Error;
            }
        }
    }
    //fprintf(stderr, "fill %i %i %i\n", space, parent->currentDataBlock, n);
    return n;
}
void
printEntry(const char* d) {
    char type = d[0x42];
    string name;
    for (int i=0; i< d[0x40]; ++i) {
        name.append(d+2*i,1);
    }
    int32_t prevIndex = readLittleEndianInt32(d+0x44);
    int32_t nextIndex = readLittleEndianInt32(d+0x48);
    int32_t firstIndex = readLittleEndianInt32(d+0x4C);
    int32_t blockStart = readLittleEndianInt32(d+0x74);
    int32_t blockSize = readLittleEndianInt32(d+0x78);
    printf("entry %i %s: %i %i %i %i %i %i\n", d[0x40],name.c_str(), type, prevIndex, nextIndex, firstIndex,
        blockStart, blockSize);
}

OleInputStream::OleInputStream(InputStream* input) :SubStreamProvider(input),
        entrystream(new OleEntryStream(this)) {
    currentTableBlock = -1;
    // read start
    size = input->read(data, 512, 512);
    if (size != 512) {
        m_status = Error;
        m_error = "File is too small.";
        return;
    }
    input->reset(0);

    int32_t nBat = readLittleEndianInt32(data+0x2c);
    int32_t ptOffset = readLittleEndianInt32(data+0x30);
    int32_t sBatOffset = readLittleEndianInt32(data+0x3c);
    //int32_t xBatOffset = readLittleEndianInt32(data+0x44);
    int32_t nXBat = readLittleEndianInt32(data+0x48);
    if (!checkHeader(data, size) || nBat < 0 || nBat > 109 || nXBat < 0) {
        m_status = Error;
        m_error = "Invalid OLE file.";
        return;
    }
//    fprintf(stderr, "%i %i %i %i\n", nBat, ptOffset, sBatOffset, nXBat);
//    fprintf(stderr, "doing ole\n");
    int32_t max = 0;
    batIndex.reserve(109);
    data += 76;
    for (int i = 0; i < nBat; ++i) {
        int32_t p = readLittleEndianInt32(data+4*i);
        batIndex.push_back(p);
        if (p > max) max = p;
    }
    if (ptOffset > max) max = ptOffset;

    int32_t toread = (max+2)*512;
    if (input->size() >= 0 && input->size() < toread) {
        m_status = Error;
        m_error = "File is incomplete.";
        return;
    }
    toread = (input->size() > 0) ?input->size() :10000000;
    size = input->read(data, toread, toread);
    if (size != input->size()) {
        m_status = Error;
        m_error = string("File cannot be read completely: ")+input->error();
        return;
    }
    maxindex = size/512-2;

    // print all bat blocks
/*    for (int i = 0; i<nBat; ++i) {
        const char* b = data+(1+batIndex[i])*512;
        fprintf(stderr, "%4.i\n", batIndex[i]);
        for (int j=0; j<128;++j) {
            int32_t p = readLittleEndianInt32(b+4*j);
            fprintf(stderr, "%4.i ", p);
            if (j%16 == 15) {fprintf(stderr, "\n");}
        }
    }*/

    // collect all sbat blocks
//    fprintf(stderr, "sbat blocks\n");
    while (sBatOffset >= 0 && sbatIndex.size() < 1000) {
        sbatIndex.push_back(sBatOffset);
/*        const char* b = data+(1+sBatOffset)*512;
        fprintf(stderr, "%4.i\n", sBatOffset);
        for (int j=0; j<128;++j) {
            int32_t p = readLittleEndianInt32(b+4*j);
            fprintf(stderr, "%4.i ", p);
            if (j%16 == 15) {fprintf(stderr, "\n");}
        }*/
        sBatOffset = nextBlock(sBatOffset);
    }

    sbatbIndex.reserve(sbatIndex.size()*16);
    // read the info for the root entry
    currentDataBlock = readLittleEndianInt32(data + (1+ptOffset)*512 + 0x74);
//    fprintf(stderr, "db %i\n", currentDataBlock);
    while (currentDataBlock >= 0 && sbatbIndex.size() < 16000) {
        sbatbIndex.push_back(currentDataBlock);
        currentDataBlock = nextBlock(currentDataBlock);
    }
    maxsindex = sbatbIndex.size()*8;
//    fprintf(stderr, "i sb %i %i max: %i\n", sbatIndex.size(), sbatbIndex.size(), maxsindex);

    currentTableBlock = ptOffset;
    currentTableIndex = 0;
}
OleInputStream::~OleInputStream() {
    delete entrystream;
}
int32_t
OleInputStream::nextBlock(int32_t in) {
    // get the number of the bat block we need
    uint32_t bid = in/128;
    if (bid < 0 || bid >= batIndex.size()) {
        fprintf(stderr, "error: input block out of range %i\n", in);
        return -4;
    }
    bid = batIndex[bid]+1;
    int32_t next = in%128*4;
    next = readLittleEndianInt32(data+512*bid+next);
    bool error = next < -2 || next == -1 || next > maxindex;
    if (error) {
        fprintf(stderr, "error: output block out of range %i\n", next);
    }
    return (error) ?-4 :next;
}
int32_t
OleInputStream::nextSmallBlock(int32_t in) {
    // get the number of the sbat block we need
    uint32_t bid = in/128;
    if (bid < 0 || bid >= sbatIndex.size()) {
        fprintf(stderr, "error: input block out of range %i\n", in);
        return -4;
    }
    bid = sbatIndex[bid]+1;
    int32_t next = in%128*4;
    next = readLittleEndianInt32(data+512*bid+next);
    bool error = next < -2 || next == -1 || next > maxsindex;
    if (error) {
        fprintf(stderr, "error: output block out of range %i\n", next);
    }
    return (error) ?-4 :next;
}
const char*
OleInputStream::getCurrentSmallBlock() {
    const char* d = data;
    // each block of 512 has 8 blocks of 64
    int32_t i = currentDataBlock/8;
    if (i < 0 || i > (int32_t)sbatbIndex.size()) {
        return 0;
    }
    i = 512*(1+sbatbIndex[i]) + (currentDataBlock%8)*64;
    return (i > size-64) ?0 :d+i;
}
void
OleInputStream::readEntryInfo() {
    const char* d = data + (1+currentTableBlock)*512 + 128*currentTableIndex;
    char entryType = d[0x42];
    if (entryType != 2) {
        currentDataBlock = -1;
        return;
    }
    string name;
    int32_t namesize = ((d[0x40]>0x40) ?0x40 :d[0x40])/2 - 1;
    name.resize(namesize);
    for (int i=0; i < namesize; ++i) {
        name[i] = d[2*i];
    }
    // only allow valid Utf8 names or names that start with the value 5
    // TODO: handle names that start with 0x1
/*    if (namesize == 0 || (name[0] != 5 && !checkUtf8(name))) {
        fprintf(stderr, "Invalid entry name in OLE: '%s' of length %i\n",
	    name.c_str(), namesize);
        currentDataBlock = -1;
        return;
    }*/
    
    m_entryinfo.filename.assign(name);
    currentDataBlock = readLittleEndianInt32(d+0x74);
    currentStreamSize = readLittleEndianInt32(d+0x78);
    m_entryinfo.size = currentStreamSize;
    if (currentDataBlock > maxindex || currentStreamSize <= 0) {
        currentDataBlock = -1;
    }
}
InputStream*
OleInputStream::nextEntry() {
    if (currentTableBlock < 0) return 0;
    do {
        if (++currentTableIndex == 4) {
            currentTableBlock = nextBlock(currentTableBlock);
            if (currentTableBlock < 0) {
                return 0;
            }
            currentTableIndex = 0;
        }
        readEntryInfo();
    } while (currentDataBlock < 0);
    //printEntry(data+(currentTableBlock+1)*512+128*currentTableIndex);

    entrystream->reset();

    return entrystream;
}
bool
OleInputStream::checkHeader(const char* data, int32_t datasize) {
    static const unsigned char ole_magic[] = {
        0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };
    return datasize > 8 && memcmp(data, ole_magic, 8) == 0;
}
