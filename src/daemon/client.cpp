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
/* a client in the unix domain */
#include "socketclient.h"

int
main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage %s query\n", argv[0]);
        return -1;
    }
    SocketClient c;
    c.setSocketName("/tmp/katsocket");
    std::vector<std::string> files = c.query(argv[1]);
    if (files.size() == 0) {
        printf("Error: %s\n", c.getError().c_str());
    } else {
        for (uint i=0; i<files.size(); ++i) {
            printf("%i\t%s\n", i, files[i].c_str());
        }
    }
}

