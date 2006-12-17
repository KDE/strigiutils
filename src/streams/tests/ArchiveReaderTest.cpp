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

#include "../archivereader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

using namespace jstreams;
using namespace std;

void
test1(const char* fullpath) {
    FileStreamOpener opener;
    ArchiveReader r;
    r.addStreamOpener(&opener);
    StreamBase<char>* s = r.openStream(fullpath);
    r.closeStream(s);
}

void
walkdirectories(const char* path, void (*callback)(const char*)) {
    DIR* dir = opendir(path);
    if (dir == 0) return;
    string p(path);
    struct dirent* subdir = readdir(dir);
    struct stat dirstat;
    while (subdir) {
        if (subdir->d_name[0] == '.') {
            subdir = readdir(dir);
            continue;
        }
        string name = subdir->d_name;
        string filepath = p + name;
        if (lstat(filepath.c_str(), &dirstat) == 0) {
            if (S_ISREG(dirstat.st_mode)) {
                callback(filepath.c_str());
            } else if (S_ISDIR(dirstat.st_mode)) {
                filepath += "/";
                walkdirectories(filepath.c_str(), callback);
            }
        }
        subdir = readdir(dir);
    }
    closedir(dir);
}

/**
 * Test the class ArchiveReader by analyzing all files in the given
 * directory.
 **/
int
ArchiveReaderTest(int argc, char** argv) {
    if (argc < 2) return 1;
    printf("%s\n", argv[1]);
    walkdirectories(argv[1], test1);
    return 0;
}
