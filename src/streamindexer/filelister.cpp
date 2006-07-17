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
#include "stgdirent.h" //dirent replacement (includes native if available)

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#include <errno.h>
#ifndef PATH_MAX
 #define PATH_MAX _MAX_PATH
#endif

void
FileLister::listFiles(const char *dir, time_t oldestdate) {
    if (m_callback == 0) return;
    m_oldestdate = oldestdate;
    int len = strlen(dir);
    // check that the dirname ends in '/'
    if (dir[len-1] != '/') {
        return;
    }
    walk_directory(dir, len);
}
/**
 * Walk through a directory. The directory name must end in a '/'.
 **/
bool
FileLister::walk_directory(const char *dirname, int len) {
    bool expandedPath = false;
    DIR *dir;
    struct dirent *subdir;
    struct stat dirstat;

    /* The full path of the current file. todo: how to determine the max
       expanded path? */
    char cwd[PATH_MAX*32];
    strcpy(cwd, dirname);

    // remove the trailing '/' on windows machines
#ifdef _WIN32
    // dont strip off the trailing slash from windows c:/
    if ( len > 3) {
        cwd[len-1] = 0;
        len--;
    }
#endif

    // open the directory
    dir = opendir(cwd);
    if (dir == 0) {
        return true;
    }

    // a pointer past the last character in the current directory name
    char* pcwd = cwd + len;
    // add the trailing '/'
    if ( *(pcwd-1) != '/' ) {
        *pcwd = '/';
        pcwd++;
    }

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

        int l = strlen(subdir->d_name);
        strcpy(pcwd, subdir->d_name);
        if (stat(cwd, &dirstat) == 0) {
            bool c = true;
            if ( dirstat.st_mode & S_IFREG
                    && dirstat.st_mtime >= m_oldestdate) {
                c = m_callback(dirname, subdir->d_name, dirstat.st_mtime);
            } else if ( dirstat.st_mode & S_IFDIR ) {
                strcat(pcwd+l, "/");
                c = walk_directory(cwd, len+l+1);
            }
            if (!c) break;
        } else {
            fprintf(stderr, "Could not stat '%s': %s\n", cwd, strerror(errno));
        }
        
        subdir = readdir(dir);
    }

    // clean up
    closedir(dir);
    // go back to where we came from
    chdir("..");
    return true;
}
