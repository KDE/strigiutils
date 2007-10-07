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
#include <stdio.h>

#include <strigi/strigiconfig.h>
#include "filelister.h"
#include "analyzerconfiguration.h"
#include "strigi_thread.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>

using namespace std;

class TaskList {
private:
    list<string> tasks;
    STRIGI_MUTEX_DEFINE(mutex);
    unsigned int busytasks;
public:
    TaskList() :busytasks(0) {
        STRIGI_MUTEX_INIT(&mutex);
    }
    ~TaskList() {
        STRIGI_MUTEX_DESTROY(&mutex);
    }
    void addTask(const string& path);
    void startTask(string& path);
    void endTask();
};
void
TaskList::addTask(const string& path) {
    STRIGI_MUTEX_LOCK(&mutex);
    if (path.length()) {
        tasks.push_back(path);
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
}
void
TaskList::startTask(string& path) {
    STRIGI_MUTEX_LOCK(&mutex);
    if (!tasks.empty()) {
        path.assign(tasks.front());
        tasks.pop_front();
        busytasks++;
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
}
void
TaskList::endTask() {
    STRIGI_MUTEX_LOCK(&mutex);
    busytasks--;
    STRIGI_MUTEX_UNLOCK(&mutex);
}

/**
 * Class to go through directories in a threaded fashion.
 **/
class DirLister {
private:
    TaskList tasklist;
    const Strigi::AnalyzerConfiguration* const config;
public:
    DirLister(const Strigi::AnalyzerConfiguration* c = 0) :config(c) {}
    virtual ~DirLister() {}
    int nextDir(std::string& path,
        std::vector<std::pair<std::string, time_t> >& dirs);
};
int
DirLister::nextDir(std::string& path,
        std::vector<std::pair<std::string, time_t> >& dirs) {
    dirs.clear();
    // get the next dir from the list
    //
    // todo: if path.length() == 0 and another thread is still in this function,
    // wait until it is done and check again
    tasklist.startTask(path);
    if (path.length() <= 0) {
        return 1;
    }
    DIR* dir = opendir(path.c_str());
    if (dir == 0) {
        return 1;
    }
    string fullpath(path);
    string::size_type len = fullpath.length();
    if (path[len-1] != '/') {
        fullpath.append("/");
        len++;
    }
    struct stat dirstat;
    struct dirent* ent = readdir(dir);
    string entname;
    while (ent) {
        // skip the directories '.' and '..'
        char c1 = ent->d_name[0];
        if (c1 == '.') {
            char c2 = ent->d_name[1];
            if (c2 == '.' || c2 == '\0') {
                ent = readdir(dir);
                continue;
            }
        }
        entname.assign(ent->d_name);
        fullpath.resize(len);
        fullpath.append(entname);
#ifdef _WIN32
        // windows does not have symbolic links, so stat() is fine
        if (stat(fullpath.c_str(), &dirstat) == 0) {
#else
        if (lstat(fullpath.c_str(), &dirstat) == 0) {
#endif
            if (S_ISREG(dirstat.st_mode)) {
                if (config == 0 || config->indexFile(fullpath.c_str(),
                        fullpath.c_str()+len)) {
                    dirs.push_back(make_pair(entname, dirstat.st_mtime));
                }
            } else if (dirstat.st_mode & S_IFDIR && (config == 0
                    || config->indexDir(fullpath.c_str(),
                                        fullpath.c_str()+len))) {
                tasklist.addTask(fullpath);
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
    tasklist.endTask();
    return 0;
}
int
main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }

    DirLister d;


    return 0;
}
