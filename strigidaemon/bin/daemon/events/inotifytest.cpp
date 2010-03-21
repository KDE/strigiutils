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
#include <sys/inotify.h>
#include <list>
#include <string>

// http://www.linuxjournal.com/article/8478

class INotifyWatcher {
private:
    static const int m_ievents;
    static int m_inotifyfd;
    std::list<std::string> dirs;

//    void bootstrapDir(const std::string& dir);
    void handleEvent(const struct inotify_event& event);
    bool handleEvents();

    static void indexFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime);
    static void watchDirCallback(const char* path, uint len);
public:
    INotifyWatcher();
    void addDir(const std::string& dir);
    void watch();
};

#include "filelister.h"
#include <strigi/analyzerconfiguration.h>
#include <errno.h>
using namespace std;
using namespace Strigi;

int INotifyWatcher::m_inotifyfd;
const int INotifyWatcher::m_ievents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM
        | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;

INotifyWatcher::INotifyWatcher() {
    m_inotifyfd = inotify_init();
    if (m_inotifyfd < 0) {
        fprintf(stderr, "INotify cannot be initialized.\n");
    }
}
void
INotifyWatcher::addDir(const std::string& dir) {
    AnalyzerConfiguration ic;
    FileLister lister(ic);
    lister.setFileCallbackFunction(&indexFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);
    lister.listFiles(dir.c_str());
}
void
INotifyWatcher::indexFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime) {
}
void
INotifyWatcher::watchDirCallback(const char* path, uint len) {
    int wd = inotify_add_watch(m_inotifyfd, path, m_ievents);
    if (wd < 0) {
        if (errno == ENOSPC) {
            fprintf(stderr, "No space for more watches.\n");
        }
        return;
    }
    printf("%s %i\n", path, wd);
}
#include <sys/ioctl.h>
void
INotifyWatcher::watch() {
    fd_set read_fds;
    struct timeval timeout;
    timeout.tv_sec = 1000000;
    timeout.tv_usec = 0;

    do {
        FD_ZERO(&read_fds);
        FD_SET(m_inotifyfd, &read_fds);
        int rc = select(m_inotifyfd+1, &read_fds, NULL, NULL, &timeout);
        if (rc < 0) {
            fprintf(stderr, "select on inotify failed\n");
            return;
        }
        printf("handle events\n");
    } while(handleEvents());
}
bool
INotifyWatcher::handleEvents() {
    /* size of the event structure, not counting name */
    #define EVENT_SIZE  (sizeof (struct inotify_event))
    /* reasonable guess as to size of 1024 events */
    #define BUF_LEN        (1024 * (EVENT_SIZE + 16))
    char buf[BUF_LEN];
    ssize_t len, i = 0;
    len = read (m_inotifyfd, buf, BUF_LEN);
    printf("handle events %i\n", len);
    if (len > 0) {
        while (i < len) {
            struct inotify_event *event;
            event = (struct inotify_event *) &buf[i];
            handleEvent(*event);
            printf ("wd=%d mask=%u cookie=%u len=%u\n",
                event->wd, event->mask,
                event->cookie, event->len);
            if (event->len)
                printf ("name=%s\n", event->name);
            i += EVENT_SIZE + event->len;
        }
    }
    return len > 0;
}
void
INotifyWatcher::handleEvent(const struct inotify_event& event) {
    printf("event! %s\n", event.name);
}

int
main(int argc, char** argv) {
    INotifyWatcher watcher;
    watcher.addDir("/tmp");
    watcher.addDir("/tmp/b");
    for (int i=1; i<argc; i++) {
        watcher.addDir(argv[i]);
    }
    watcher.watch();
    
    return 0;
}
