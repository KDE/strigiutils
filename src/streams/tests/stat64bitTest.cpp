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
#include "config.h"

#include <stdio.h>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>
using namespace std;

/*
 * Test to check if the file size returned from stat() can handle large files
 **/
int
stat64bitTest(int argc, char* argv[]) {

    if (sizeof(off_t) < 8) return 1;

    // create a file of 5 GB
    FILE* file = tmpfile();
    if (file == 0) return 1;
    off_t size = 5368709120ll;
    if (fseeko(file, size, SEEK_SET)) return 1;
    if (fwrite("hi", 1, 2, file) != 2) return 1;
    if (fflush(file)) return 1;

    // check the filesize
    int fd = fileno(file);
    if (fd == -1) return 1;
    struct stat s;
    if (fstat(fd, &s)) return 1;
    if (s.st_size != 5368709122ll) return 1;
    if (fclose(file)) return 1;
    return 0;
}

