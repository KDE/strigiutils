/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
#include "strigiconfig.h"
#include "archivereader.h"
#include "fileinputstream.h"
#include "substreamprovider.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace Strigi;
using namespace std;

int
main(int argc, char** argv) {
    EntryInfo e;
    FileStreamOpener opener;

    ArchiveReader reader;
    reader.addStreamOpener(&opener);

    for (int i=1; i<argc; ++i) {
        EntryInfo e;
        if (reader.stat(argv[i], e) != 0) {
            printf("could not read %s\n", argv[i]);
            continue;
        }
        printf("file: %i %s\n", e.type, e.filename.c_str());
        DirLister dl = reader.dirEntries(argv[i]);
        while (dl.nextEntry(e)) {
            printf("%i %s\n", e.type, e.filename.c_str());
        }
        StreamBase<char>* s = 0;
        if (e.type & EntryInfo::File) {
            s = reader.openStream(argv[i]);
        }
        if (s) {
            const char* c;
            int64_t total = 0;
            int32_t n = s->read(c, 1, 0);
            while (n > 0) {
                total += n;
                n = s->read(c, 1, 0);
            }
            cout << "read " << total << " bytes." << endl;
            if (s->status() == Error) {
                printf("Error: %s\n", s->error());
            }
            reader.closeStream(s);
        }
    }
    return 0;
}

