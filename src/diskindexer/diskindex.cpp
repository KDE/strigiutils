/***************************************************************************
 *   Copyright (C) 2006 by Jos van den Oever   *
 *   jos@vandenoever.info   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include "fileinputstream.h"
#include "gzipinputstream.h"
#include "bz2inputstream.h"
#include "zipinputstream.h"
#include "tarinputstream.h"
#include "subinputstream.h"
#include "streamindexer.h"

using namespace std;
using namespace jstreams;

long
streamLen(InputStream *i) {
    long count = 0;
    const char *begin;
    int32_t nread;
    char r = i->read(begin, nread);
    while (r == 0) {
        count += nread;
//        printf ("count %li\n", count);
        r = i->read(begin, nread, 1);
    }
    if (r == -2) count = -2;
    return count;
}

void
analyze(InputStream *i, const char *path) {
//    bool isgz = true;
    long count;
    i->mark(1024);
    GZipInputStream gz(i);
    count = streamLen(&gz);
    if (count == -2) {
        i->reset();
        count = streamLen(i);
    }
    fprintf(stderr, "read %li bytes from file %s\n", count, path);
}
void
printSigPositions(InputStream *i) {
    const char *begin;
    int32_t nread;
    i->read(begin, nread);
    printf("read: %i\n", nread);
    if (nread < 4) return;
    for (int32_t j=0; j<nread-4; j++) {
        if (begin[j]==0x50&&begin[j+1]==0x4b&&begin[j+2]==0x03&&begin[j+3]==0x04) {
            printf("pos: %i\n", j);
        }
    }
    
 /*   const char *pos = begin;
    while (pos) {
        pos = strstr(pos, sig);
        if (pos) {
            printf("pos: %i\n", pos-begin);
            pos += 1;
        }
    }*/
}
void
unzip(InputStream &i, const char *path) {
    printf ("archive: %s\n", path);
    ZipInputStream zip(&i);
    InputStream *entrystream = zip.nextEntry();
    while (entrystream) {
        long l = streamLen(entrystream);
        printf ("%li\n", l);
        //analyze(entrystream, zip.getEntryFileName().c_str());
        entrystream = zip.nextEntry();
    }
    if (zip.getError().size()) {
        printf("error: %s\n", zip.getError().c_str());
    }
}
void
listtar(InputStream *i, const char *path) {
    printf ("archive: %s\n", path);
    TarInputStream tar(i);
    InputStream *entrystream = tar.nextEntry();
    while (entrystream) {
        long l = streamLen(entrystream);
        printf ("%li\n", l);
        //analyze(entrystream, zip.getEntryFileName().c_str());
        entrystream = tar.nextEntry();
    }
    if (tar.getError().size()) {
        printf("error: %s\n", tar.getError().c_str());
    }
}
void
bunzip(InputStream &i, const char *path) {
    GZipInputStream bz2(&i);
    listtar(&bz2, path);
//    long l = streamLen(&bz2);
//    printf("%li\n", l);
    //GZipInputStream gz(&i);
}

void
index(const char *path) {
    StreamIndexer indexer;
    indexer.indexFile(path);
}
int main(int argc, char *argv[])
{
    for (int i = 1; i<argc; i++)
    {
        index(argv[i]);
    }
    for (int i = 1; i<argc; i++)
    {
        FileInputStream file(argv[i]);
//        listtar(&file, argv[i]);
    }
    return EXIT_SUCCESS;
}
