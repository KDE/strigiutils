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

#ifndef PATH_MAX
 #define PATH_MAX _MAX_PATH
#endif

void
FileLister::listFiles(const char *dir, time_t oldestdate) {
    if (m_callback == 0) return;
    m_oldestdate = oldestdate;
    walk_directory(dir);
}
bool
FileLister::walk_directory(const char *dirname) {
    bool expandedPath = false;
    DIR *dir;
    struct dirent *subdir;
    struct stat dirstat;

    char cwd[PATH_MAX*32]; //the full path of the current file. todo: how to determine the max expanded path?
    strcpy(cwd,dirname);
    int len = strlen(cwd);
    if ( len > 2 && (cwd[len-1] == '/' || cwd[len-1] == '\\') ) {
#ifdef _WIN32
        if ( len > 3 ){ //dont strip off the trailing slash from windows c:/
            cwd[len-1] = 0;
            len--;
        }
#else
    cwd[len-1] = 0;
#endif
    }

    // open the directory
    dir = opendir(cwd);
    if (dir == 0) {
        return true;
    }

    char* pcwd = cwd + len; //a pointer to the end of the current directory
    if ( *(pcwd-1) != '/' && *(pcwd-1) != '\\' ){//make sure we have a trailing /
        *pcwd = '/';
        pcwd++;
    }
    int cwd_rem = PATH_MAX - (pcwd-cwd);

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

        strcpy(pcwd, subdir->d_name);
        if (stat(cwd, &dirstat) == 0) {
            bool c = true;
            if ( dirstat.st_mode & S_IFREG
                    && dirstat.st_mtime >= m_oldestdate) {
                c = m_callback(dirname, subdir->d_name, dirstat.st_mtime);
            } else if ( dirstat.st_mode & S_IFDIR ) {
                strcat(cwd,"/");
                c = walk_directory(cwd);
            }
            if (!c) break;
        }
        
        subdir = readdir(dir);
    }

    // clean up
    closedir(dir);
    // go back to where we came from
    chdir("..");
    return true;
}
