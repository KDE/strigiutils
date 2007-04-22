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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "strigiconfig.h"
#include "filelister.h"
#include "analyzerconfiguration.h"
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
    if ( path.c_str() == NULL || path.length() == 0 )
        return "";

    string temp(path);

#ifdef HAVE_WINDOWS_H
    size_t l= temp.length();
    char* t = (char*)temp.c_str();
    for (size_t i=0;i<l;i++){
        if ( t[i] == '\\' )
            t[i] = '/';
    }
    temp[0] = tolower(temp.at(0));
#endif

    char separator = '/';

    if (temp[temp.length() - 1 ] != separator)
        temp += separator;

    return temp;
}

FileListerDep::FileListerDep(Strigi::AnalyzerConfiguration& ic) :m_config(ic) {
    m_fileCallback = 0;
    m_dirCallback = 0;
    path = 0;
    length = 0;
#ifdef _WIN32
    uid = 0;
    gid = 0;
#else
    uid = geteuid();
    gid = getegid();
#endif
}
FileListerDep::~FileListerDep() {
    if (length) {
        free(path);
    }
}
void
FileListerDep::listFiles(const char *dir, time_t oldestdate) {
    if (m_fileCallback == 0) return;
    fprintf(stderr, "listFiles %s\n", dir);
    m_oldestdate = oldestdate;
    int len = strlen(dir);
    resize(len+2);
    strcpy(path, dir);
    // check that the dirname ends in '/'
    if (path[len-1] != '/') {
        strcpy(path+len, "/");
        len++;
    }
#ifdef WIN32
    path[0] = tolower(path[0]);
#endif
    walk_directory(len);
}
char*
FileListerDep::resize(uint len) {
    if (len > length) {
        length = len + 100;
        path = (char*) realloc(path, length);
    }
    return path;
}
/**
 * Walk through a directory. The directory name must end in a '/'.
 **/
void
FileListerDep::walk_directory(uint len) {
    //bool expandedPath = false;
    DIR *dir;
    struct dirent *subdir;
    struct stat dirstat;

#ifdef WIN32
    // windows opendir expects no trailing slash on the end of directories...
    // remove the trailing '/' on windows machines before the call to opendir(),
    // but do not strip off the trailing slash from windows c:/
    if ( len > 3) {
        while(path[len-1]=='/' || path[len-1]=='\\') {
            len--;
            path[len] = '\0';
        }
    }
#endif

    // call dir function callback, actually there's only inotify dir callback
    if (m_dirCallback != 0)
        m_dirCallback(path, len);

    // open the directory
    dir = opendir(path);
    if (dir == 0) {
        return;
    }
#ifdef WIN32
    //add the trailing slash back on
    if ( len > 3) {
        strcat(path, "/");
        len++;
    }
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

        uint l = len + strlen(subdir->d_name);
        path = resize(l+1);
        strcpy(path+len, subdir->d_name);
        // check if the file is a normal file (use lstat, NOT stat)
#ifdef _WIN32
        if (_stat(path, (struct _stat*)&dirstat) == 0 && (S_IROTH & dirstat.st_mode
                || (uid == dirstat.st_uid && S_IRUSR & dirstat.st_mode)
                || (gid == dirstat.st_uid && S_IRGRP & dirstat.st_mode))) {
            if (S_ISREG(dirstat.st_mode) && dirstat.st_mtime >= m_oldestdate) {
#else
        if (lstat(path, &dirstat) == 0) {
            if (S_ISREG(dirstat.st_mode) && dirstat.st_mtime >= m_oldestdate) {
#endif
                if (m_config.indexFile(path, path+len)) {
                    m_fileCallback(path, len, l, dirstat.st_mtime);
                }
            } else if (dirstat.st_mode & S_IFDIR
                    && m_config.indexDir(path, path+len)) {
                // append a '/' to the path
                strcpy(path+l, "/");
                walk_directory(l+1);
            }
        }

        subdir = readdir(dir);
    }

    // clean up
    closedir(dir);
    // go back to where we came from
    //chdir("..");
}
class Strigi::FileLister::Private {
public:
    char path[10000];
    pthread_mutex_t mutex;
    DIR** dirs;
    DIR** dirsEnd;
    DIR** curDir;
    int* len;
    int* lenEnd;
    int* curLen;
    time_t mtime;
    struct dirent* subdir;
    struct stat dirstat;
    const Strigi::AnalyzerConfiguration* const config;

    Private(const Strigi::AnalyzerConfiguration* ic);
    ~Private();
    int nextFile(string& p, time_t& time) {
        int r;
        pthread_mutex_lock(&mutex);
        r = nextFile();
        if (r > 0) {
            p.assign(path, r);
            time = mtime;
        }
        pthread_mutex_unlock(&mutex);
        return r;
    }
    void startListing(const std::string&);
    int nextFile();
};
Strigi::FileLister::Private::Private(
            const Strigi::AnalyzerConfiguration* ic) :
        config(ic) {
    pthread_mutex_init(&mutex, 0);
    int nOpenDirs = 100;
    dirs = (DIR**)malloc(sizeof(DIR*)*nOpenDirs);
    dirsEnd = dirs + nOpenDirs;
    len = (int*)malloc(sizeof(int)*nOpenDirs);
    lenEnd = len + nOpenDirs;
}
void
Strigi::FileLister::Private::startListing(const string&dir){
    curDir = dirs;
    curLen = len;
    int len = dir.length();
    *curLen = len;
    strcpy(path, dir.c_str());
    if (len) {
        if (path[len-1] != '/') {
            path[len++] = '/';
            path[len] = 0;
            *curLen = len;
        }
        DIR* d = opendir(path);
        if (d) {
            *curDir = d;
        } else {
            curDir--;
        }
    } else {
        curDir--;
    }
}
Strigi::FileLister::Private::~Private() {
    fprintf(stderr, "~FileLister\n");
    while (curDir >= dirs) {
        if (*curDir) {
            closedir(*curDir);
        }
        curDir--;
    }
    free(dirs);
    free(len);
    pthread_mutex_destroy(&mutex);
}
int
Strigi::FileLister::Private::nextFile() {
    //fprintf(stderr, "cD %i\n", curDir-dirs);

    while (curDir >= dirs) {
        DIR*& dir = *curDir;
        int l = *curLen;
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
            strcpy(path + l, subdir->d_name);
            int sl = l + strlen(subdir->d_name);
            //printf("read %i %s\n", sl, path);
            if (lstat(path, &dirstat) == 0) {
                if (S_ISREG(dirstat.st_mode)) {
                    return sl;
                } else if (dirstat.st_mode & S_IFDIR) {
                    strcpy(this->path+sl, "/");
                    DIR* d = opendir(path);
                    if (d) {
                        curDir++;
                        *curDir = d;
                        curLen++;
                        *curLen = sl+1;
                    }
                }
            }
            subdir = readdir(dir);
        }
        closedir(dir);
        curDir--;
        curLen--;
    }
    return -1;
}
Strigi::FileLister::FileLister(const Strigi::AnalyzerConfiguration* ic)
    : p(new Private(ic)) {
}
Strigi::FileLister::~FileLister() {
    delete p;
}
void
Strigi::FileLister::startListing(const string& dir) {
    p->startListing(dir);
}
int
Strigi::FileLister::nextFile(std::string& path, time_t& time) {
    return p->nextFile(path, time);
}
int
Strigi::FileLister::nextFile(const char*& path, time_t& time) {
    int r = p->nextFile();
    if (r >= 0) {
        time = p->mtime;
        path = p->path;
    }
    return r;
}
