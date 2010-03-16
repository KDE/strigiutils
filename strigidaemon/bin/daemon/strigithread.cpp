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
#include "strigithread.h"
#include <strigi/strigi_thread.h>
#include "strigilogging.h"

#include <string>
#include <cstring>
#include <cstdio>
#include <errno.h>
#include <signal.h>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/syscall.h>

// define two enums and a constant for use of ioprio
enum {
    IOPRIO_CLASS_NONE,
    IOPRIO_CLASS_RT,
    IOPRIO_CLASS_BE,
    IOPRIO_CLASS_IDLE
};

enum {
    IOPRIO_WHO_PROCESS = 1,
    IOPRIO_WHO_PGRP,
    IOPRIO_WHO_USER
};
#define IOPRIO_CLASS_SHIFT  13

using namespace std;

vector<StrigiThread*> threads;

void
StrigiThread::stopThreads() {
    vector<StrigiThread*>::const_iterator i;
    for (i=threads.begin(); i!=threads.end(); ++i) {
        STRIGI_LOG_INFO ("strigi.daemon", string("stopping thread ")
            + (*i)->name);
        (*i)->stop();
        STRIGI_LOG_INFO ("strigi.daemon", "stopped another thread ");
    }
}

extern "C" void
quit_daemon(int signum) {
    STRIGI_LOG_INFO("strigi.daemon", "quit_daemon");
    static int interruptcount = 0;
    vector<StrigiThread*>::const_iterator i;
    switch (++interruptcount) {
    case 1:
        STRIGI_LOG_INFO ("strigi.daemon", "stopping threads ");
        StrigiThread::stopThreads();
        break;
    case 2:
        STRIGI_LOG_INFO ("strigi.daemon", "terminating threads ");
        for (i=threads.begin(); i!=threads.end(); ++i) {
            (*i)->terminate();
        }
        break;
    case 3:
    default:
        STRIGI_LOG_FATAL ("strigi.daemon", "calling exit(1)")
        exit(1);
    }
}

struct sigaction quitaction;
void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}
struct sigaction dummyaction;
extern "C" void nothing(int) {}
void
set_wakeup_on_signal(int signum) {
    dummyaction.sa_handler = nothing;
    sigaction(signum, &dummyaction, 0);
}

extern "C" void*
threadstarter(void *d) {
    // give this thread job batch job priority
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = 0;
    StrigiThread* thread = static_cast<StrigiThread*>(d);

#ifndef __APPLE__
    if (thread->getPriority() > 0) {
#ifndef SCHED_BATCH
#define SCHED_BATCH 3
#endif
        // renice the thread
        int r  = setpriority(PRIO_PROCESS, 0, thread->getPriority());
        if (r != 0) {
            STRIGI_LOG_ERROR (string("strigi.daemon.") + thread->name
                + ".threadstarter",
                string("error setting priority: ") + strerror(errno));
        }
        r = sched_setscheduler(0, SCHED_BATCH, &param);
        if (r != 0) {
            STRIGI_LOG_INFO (string("strigi.daemon.") + thread->name
                + ".threadstarter",
                string("error setting to batch: ") + strerror(errno));
        }
#ifdef SYS_ioprio_set
        if (syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0,
                IOPRIO_CLASS_IDLE<<IOPRIO_CLASS_SHIFT ) < 0 ) {
            fprintf(stderr, "cannot set io scheduling to idle (%s). "
                "Trying best effort.\n",  strerror( errno ));
            if (syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0,
                    7|IOPRIO_CLASS_BE<<IOPRIO_CLASS_SHIFT ) < 0 ) {
                fprintf( stderr, "cannot set io scheduling to best effort.\n");
            }
        }
#endif
    }
#endif

    // start the actual work
    thread->run(0);
    STRIGI_LOG_DEBUG(string("strigi.daemon.") + thread->name + ".threadstarter", "end of thread");
    STRIGI_THREAD_EXIT(0);
    return 0;
}

StrigiThread::StrigiThread(const char* n) :state(Idling),thread(0),  name(n) {
    priority = 0;
    STRIGI_MUTEX_INIT(&lock);
}
StrigiThread::~StrigiThread() {
    STRIGI_MUTEX_DESTROY(&lock);
}
void
StrigiThread::setState(State s) {
    STRIGI_MUTEX_LOCK(&lock);
    state = s;
    STRIGI_MUTEX_UNLOCK(&lock);
}
StrigiThread::State
StrigiThread::getState() {
    State s;
    STRIGI_MUTEX_LOCK(&lock);
    s = state;
    STRIGI_MUTEX_UNLOCK(&lock);
    return s;
}
std::string
StrigiThread::getStringState() {
    State s = getState();
    std::string str;
    switch (s) {
    case Idling:
        str = "idling";
        break;
    case Working:
        str = "working";
        break;
    case Stopping:
        str = "stopping";
        break;
    }
    return str;
}
int
StrigiThread::start(int prio) {
    // set up signal handling
    set_quit_on_signal(SIGINT);
    set_quit_on_signal(SIGQUIT);
    set_quit_on_signal(SIGTERM);
    set_wakeup_on_signal(SIGALRM);
    threads.push_back(this);

    priority = prio;
    // start the indexer thread
    int r = STRIGI_THREAD_CREATE(&thread, threadstarter, this);
    if (r < 0) {
        STRIGI_LOG_ERROR ("strigi.daemon." + string(name),
                          "cannot create thread");
        return 1;
    }
    return 0;
}
void
StrigiThread::stop() {
    state = Stopping;
    stopThread();
    if (thread) {
        // signal the thread to wake up
        pthread_kill(thread, SIGALRM);
        // wait for the thread to finish
        STRIGI_THREAD_JOIN(thread);
    }
    thread = 0;
}
void
StrigiThread::terminate() {
    // TODO
}
