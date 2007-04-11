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
using namespace Strigi;
using namespace std;

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

OleInputStream::OleInputStream(InputStream* input) :SubStreamProvider(input) {
    // read start
    nread = input->read(data, 512, 512);
    if (nread != 512) {
        m_status = Error;
        m_error = "File is too small.";
        return;
    }
    input->reset(0);

    int32_t nBat = readLittleEndianInt32(data+44);
    int32_t benBat = readBigEndianInt32(data+44);
    int32_t ptOffset = readLittleEndianInt32(data+48);
    int32_t sBatOffset = readLittleEndianInt32(data+60);
    int32_t xBatOffset = readLittleEndianInt32(data+68);
    int32_t nXBat = readLittleEndianInt32(data+72);
    if (!checkHeader(data, nread) || nBat < 0 || nBat > 109 || nXBat < 0) {
        m_status = Error;
        m_error = "Invalid OLE file.";
        return;
    }
    fprintf(stderr, "%i\n", benBat);
    fprintf(stderr, "%i %i %i %i %i\n", nBat, ptOffset, sBatOffset, xBatOffset, nXBat);
    fprintf(stderr, "doing ole\n");
    int32_t batIndex[109];
    data += 76;
    for (int i = 0; i < nBat; ++i) {
        batIndex[i] = readLittleEndianInt32(data+4*i);
        fprintf(stderr, "%i\n", batIndex[i]);
    }

    // read the property table, this can be a very big read
    int32_t toread = (ptOffset+2)*512;
    nread = input->read(data, toread, toread);
    if (nread != toread) {
        m_status = Error;
        m_error = "File is too small.";
        return;
    }
    for (int i=0; i<8;++i) {
        printEntry(data+(ptOffset+1)*512+128*i);
    }
}
OleInputStream::~OleInputStream() {
}
InputStream*
OleInputStream::nextEntry() {
    return 0;
}
bool
OleInputStream::checkHeader(const char* data, int32_t datasize) {
    static const unsigned char ole_magic[] = {
        0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };
    return datasize > 8 && memcmp(data, ole_magic, 8) == 0;
}
