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
#include "asyncsocketclient.h"
#include <signal.h>
using namespace std;

void
quit_daemon(int i) {
    printf("signal %i\n", i);
}

struct sigaction quitaction;

void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}
AsyncSocket s;
int
main(int argc, char** argv) {
    if (argc < 2) return -1;
    set_quit_on_signal(SIGABRT);
    string query;
    for (int i=1; i<argc; ++i) {
        query += argv[i];
        query += ' ';
    }
    string homedir = getenv("HOME");
    AsyncSocketClient client;
    client.setSocketPath(homedir+"/.strigi/socket");
    bool ok = client.query(query.c_str(), -1, 0);
    if (ok) {
        while (!client.statusChanged()) {
            printf(".");
            fflush(stdout);
            struct timespec sleeptime;
            sleeptime.tv_sec = 0;
            sleeptime.tv_nsec = 1;
            nanosleep(&sleeptime, 0);
        }
        printf("hits: %i\n", client.getHits().hits.size());
    }

    return 0;
}
