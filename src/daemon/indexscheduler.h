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
#ifndef INDEXSCHEDULER_H
#define INDEXSCHEDULER_H

#include <map>
#include <string>
#include <set>
#include <pthread.h>

namespace jstreams {
    class IndexManager;
}
class IndexScheduler {
friend void* indexschedulerstart(void *);
friend bool addFileCallback(const std::string& path, const char *filename,
    time_t mtime);
private:
    enum State {Idling, Indexing, Stopping};
    State state;
    pthread_mutex_t lock;
    static pthread_mutex_t initlock;
    pthread_t thread;
    std::set<std::string> dirstoindex;
    jstreams::IndexManager* indexmanager;
    std::map<std::string, time_t> dbfiles;
    std::map<std::string, time_t> toindex;

    void* run(void*);
    void index();
public:
    static bool addFileCallback(const std::string& path, const char *filename,
        time_t mtime);
    IndexScheduler();
    void addDirToIndex(const std::string& d) {
        dirstoindex.insert(d);
    }
    void setIndexManager(jstreams::IndexManager* m) {
        indexmanager = m;
    }
    int getQueueSize();
    int start();
    void stop();
    void startIndexing() { state = Indexing; }
    void stopIndexing() { state = Idling; }
    void terminate();
    std::string getState();
    ~IndexScheduler();
    const std::set<std::string> &getIndexedDirectories() const {
        return dirstoindex;
    }
    void setIndexedDirectories(const std::set<std::string> &d);
};

#endif
