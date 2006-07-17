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
#include "sqlitestorage.h"

void
indexFile(const char *path) {
}



int
main() {
    SearchStorage *storage = new SqliteStorage("/tmp/index.db");

    int nrows = 1000;

    storage->defineProperty("md5", PropertyWriter::STRING, true);
    PropertyWriter *writer = storage->getPropertyWriter("md5");
    for (int i=0; i<nrows; i++) {
        std::string s("hiiiiiiiiiiiiiiiiiii");
        writer->writeProperty(i, s.c_str(), s.length());
        writer->writeProperty(i, s.c_str(), s.length());
    }
    storage->defineProperty("md6", PropertyWriter::STRING, true);
    writer = storage->getPropertyWriter("md6");
    for (int i=0; i<nrows; i++) {
        std::string s("hiiiiiiiiiiiiiiiiiii");
        writer->writeProperty(i, s.c_str(), s.length());
        writer->writeProperty(i, s.c_str(), s.length());
    }

    delete storage;
    return 0;
}
