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
#include "fileinputstream.h"
#include "oleinputstream.h"
using namespace Strigi;

int
main(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        fprintf(stderr, "%s\n", argv[i]);
        FileInputStream f(argv[i]);
        OleInputStream o(&f);
        InputStream* j = o.nextEntry();
        while (j) {
            const char* data;
            int64_t s = 0;
            int32_t n = j->read(data, 1, 1);
            while (n > 0) {
                s += n;
                n = j->read(data, 11111111, 11111);
            }
            fprintf(stderr, "out %lli %lli %i\n", s, j->size(), n);
            j = o.nextEntry();
        }
    }
    return 0;
}
