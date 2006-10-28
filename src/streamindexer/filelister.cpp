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
#include "filtermanager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "stgdirent.h" //dirent replacement (includes native if available)

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#include <errno.h>
#ifndef PATH_MAX
 #define PATH_MAX _MAX_PATH
#endif

using namespace std;

string fixPath (string path)
{
    string temp (path);
    
    char separator;
    
#ifdef HAVE_WINDOWS_H
    separator = '\\';
#else
    separator = '/';
#endif

    if (temp[temp.length() - 1 ] != separator)
        temp += separator;
    
    return temp;
}

FileLister::FileLister(FilterManager* filtermanager) {
    m_fileCallback = 0;
    m_dirCallback = 0;
    path = 0;
    length = 0;
    m_filterManager = filtermanager;
}
FileLister::~FileLister() {
    if (length) {
        free(path);
    }
}
#include <stdio.h>
void
FileLister::listFiles(const char *dir, time_t oldestdate) {
    if (m_fileCallback == 0) return;
    m_oldestdate = oldestdate;
    int len = strlen(dir);
    resize(len+2);
    strcpy(path, dir);
    // check that the dirname ends in '/'
    if (path[len-1] != '/') {
        strcpy(path+len, "/");
        len++;
    }
    walk_directory(len);
}
char*
FileLister::resize(uint len) {
    if (len > length) {
        length = len + 100;
        path = (char*) realloc(path, length);
    }
    return path;
}
/**
 * Walk through a directory. The directory name must end in a '/'.
 **/
bool
FileLister::walk_directory(uint len) {
    //bool expandedPath = false;
    DIR *dir;
    struct dirent *subdir;
    struct stat dirstat;
    
    if ((m_filterManager != NULL) && (m_filterManager->findMatch( path, len)))
        return true; 
    else if (m_filterManager == NULL)
        printf ("m_filtermanager is NULL!!\n");
    
#ifndef WIN32
    // call dir function callback, actually there's only inotify dir callback
    if (m_dirCallback != 0)
        m_dirCallback (path, len);
#else// remove the trailing '/' on windows machines before the call to opendir(),
    // but do not strip off the trailing slash from windows c:/
    if ( len > 3) {
        path[len-1] = '\0';
    }
#endif

    // open the directory
    dir = opendir(path);
    if (dir == 0) {
        return true;
    }
#ifdef WIN32
    path[len-1] = '/';
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

        uint l = len+strlen(subdir->d_name);
        path = resize(l+1);
        strcpy(path+len, subdir->d_name);
        if (stat(path, &dirstat) == 0) {
            bool c = true;
            if ( S_ISREG(dirstat.st_mode) && dirstat.st_mtime >= m_oldestdate) {
                if ((m_filterManager != NULL) && (!m_filterManager->findMatch( path, l)))
                    c = m_fileCallback(path, len, l, dirstat.st_mtime);
            } else if ( dirstat.st_mode & S_IFDIR) {
                strcpy(path+l, "/");
                c = walk_directory(l+1);
            }
            if (!c) break;
/*        } else {
            fprintf(stderr, "Could not stat '%s': %s\n", cwd,
                strerror(errno));*/
        }

        subdir = readdir(dir);
    }

    // clean up
    closedir(dir);
    // go back to where we came from
    //chdir("..");
    return true;
}
