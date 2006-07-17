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
#include "jstreamsconfig.h"
#include "filelister.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "stgdirent.h" //dirent replacement (includes native if available)

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#include <errno.h>
#ifndef PATH_MAX
 #define PATH_MAX _MAX_PATH
#endif

using namespace std;

FileLister::FileLister() {
    m_callback = 0;
    paths = 0;
    npaths = 0;
}
FileLister::~FileLister() {
    if (npaths == 0) return;
    for (int i=0; i<npaths; ++i) {
        if (paths[i]) free(paths[i]);
    }
    free(paths);
    free(lengths);
}
void
FileLister::setNumPaths(uint n) {
    if (paths == 0) {
        paths = (char**)calloc(n, sizeof(const char*));
        lengths = (uint*)calloc(n, sizeof(uint));
    } else {
        paths = (char**)realloc(paths, sizeof(const char*)*n);
        lengths = (uint*)realloc(lengths, sizeof(uint)*n);
        for (uint i=npaths; i<n; ++i) {
            paths[i] = 0;
            lengths[i] = 0;
        }
    }
    npaths = n;
}
void
FileLister::listFiles(const char *dir, time_t oldestdate) {
    if (m_callback == 0) return;
    m_oldestdate = oldestdate;
    int len = strlen(dir);
    // check that the dirname ends in '/'
    if (dir[len-1] != '/') {
        return;
    }
    walk_directory(0, dir, len);
}
char*
FileLister::resize(uint depth, uint len) {
    if (depth >= npaths) setNumPaths(npaths+10);
    if (lengths[depth] <= len) {
        paths[depth] = (char*)realloc(paths[depth], len+100);
        lengths[depth] = len+100;
    }
    return paths[depth];
}
/**
 * Walk through a directory. The directory name must end in a '/'.
 **/
bool
FileLister::walk_directory(uint depth, const char* dirname, uint len) {
    bool expandedPath = false;
    DIR *dir;
    struct dirent *subdir;
    struct stat dirstat;

    char* cwd = resize(depth, len);
    strcpy(cwd, dirname);

#ifdef _WIN32
    // remove the trailing '/' on windows machines,
    // but dont strip off the trailing slash from windows c:/
    if ( len > 3) {
        cwd[len-1] = '\0';
    }
#endif

    // open the directory
    dir = opendir(cwd);
    if (dir == 0) {
        return true;
    }
#ifdef _WIN32
    cwd[len-1] = '/';
#endif

    subdir = readdir(dir);
    while (subdir) {

        // skip the directories '.' and '..'
        char c1 = subdir->d_name[0];
        if (c1 == '.') {
            char c2 = subdir->d_name[1];
            if (c2 == '.' || c2 == '\0') {
                subdir = readdir(dir);
                continue;
            }
        }

        uint l = strlen(subdir->d_name);
        cwd = resize(depth, len+l+1);
        strcpy(cwd+len, subdir->d_name);
        if (stat(cwd, &dirstat) == 0) {
            bool c = true;
            if ( dirstat.st_mode & S_IFREG
                    && dirstat.st_mtime >= m_oldestdate) {
                c = m_callback(cwd, len, dirstat.st_mtime);
            } else if ( dirstat.st_mode & S_IFDIR ) {
                strcpy(cwd+len+l, "/");
                c = walk_directory(depth+1, cwd, len+l+1);
            }
            if (!c) break;
/*        } else {
            fprintf(stderr, "Could not stat '%s': %s\n", cwd,
                strerror(errno));*/
        }
        //cwd.resize(len);
        subdir = readdir(dir);
    }

    // clean up
    closedir(dir);
    // go back to where we came from
    //chdir("..");
    return true;
}
