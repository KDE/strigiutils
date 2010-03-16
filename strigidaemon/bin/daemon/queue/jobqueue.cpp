/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "jobqueue.h"
#include "job.h"
#include "strigi/strigi_thread.h"
#include <strigi/strigiconfig.h>
#include <iostream>
#include <list>
#include <errno.h>
#include <string.h>

using namespace std;

class JobThread {
private:
    STRIGI_THREAD_DEFINE(thread);
    STRIGI_MUTEX_DEFINE(mutex);
    JobQueue::Private* const queue;
    Job* job;
    bool keeprunning;

    void workloop();
    static void* start(void* vt) {
        static_cast<JobThread*>(vt)->workloop();
        return 0;
    }
public:
    JobThread(JobQueue::Private* q) :queue(q), job(0), keeprunning(true) {
        STRIGI_MUTEX_INIT(&mutex);
        STRIGI_THREAD_CREATE(&thread, JobThread::start, this);
    }
    ~JobThread() {
        STRIGI_MUTEX_DESTROY(&mutex);
    }
    void stop();
    void waitTillFinished();
};

class JobQueue::Private {
private:
   std::list<Job*> jobs;
   std::list<JobThread*> threads;
   pthread_cond_t cond;
   STRIGI_MUTEX_DEFINE(mutex);
   bool keeprunning;

public:
   Private(uint n);
   ~Private();
   bool addJob(Job* job);
   /**
    * Function for JobThreads to call to wait until a new job is available.
    * This function may return 0, but that does not mean the thread is ready.
    **/
   Job* getNextJob();
   void nudge();
};
void
JobThread::workloop() {
    bool run;
    STRIGI_MUTEX_LOCK(&mutex);
    run = keeprunning;
    STRIGI_MUTEX_UNLOCK(&mutex);
    while (run) {
        Job* j = queue->getNextJob();
        STRIGI_MUTEX_LOCK(&mutex);
        if (j) {
            job = j;
            STRIGI_MUTEX_UNLOCK(&mutex);
            j->run();
            STRIGI_MUTEX_LOCK(&mutex);
            delete job;
            job = 0;
        }
        run = keeprunning;
        STRIGI_MUTEX_UNLOCK(&mutex);
    }
    cerr << "stopping" << endl;
    STRIGI_THREAD_EXIT(&thread);
}
void
JobThread::stop() {
    STRIGI_MUTEX_LOCK(&mutex);
    keeprunning = false;
    if (job) {
        job->stop();
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
}
void
JobThread::waitTillFinished() {
    STRIGI_THREAD_JOIN(thread);
}
JobQueue::JobQueue(uint nthreads) :p(new Private(nthreads)) {}
JobQueue::~JobQueue() {
    stop();
}
bool
JobQueue::addJob(Job* job) {
    return (p) ?p->addJob(job) :false;
}
void
JobQueue::stop() {
    if (p) {
       delete p;
       p = 0;
    }
}
JobQueue::Private::Private(uint nthreads) :keeprunning(true) {
    pthread_cond_init(&cond, 0);
    STRIGI_MUTEX_INIT(&mutex);
    while (threads.size() < nthreads) {
        threads.push_back(new JobThread(this));
    }
}
JobQueue::Private::~Private() {
    STRIGI_MUTEX_LOCK(&mutex);
    keeprunning = false;
    STRIGI_MUTEX_UNLOCK(&mutex);

    // tell all threads to stop
    for (list<JobThread*>::const_iterator i = threads.begin();
            i != threads.end(); ++i) {
        (*i)->stop();
    }
    // wake up all threads
    STRIGI_MUTEX_LOCK(&mutex);
    pthread_cond_broadcast(&cond);

    // remove all remaining jobs
    for (list<Job*>::const_iterator i = jobs.begin(); i != jobs.end(); ++i) {
        delete *i;
    }
    jobs.clear();
    STRIGI_MUTEX_UNLOCK(&mutex);

    // wait for the threads to finish
    for (list<JobThread*>::const_iterator i = threads.begin();
            i != threads.end(); ++i) {
        (*i)->waitTillFinished();
        delete *i;
    }
    STRIGI_MUTEX_DESTROY(&mutex);
    pthread_cond_destroy(&cond);
}
bool
JobQueue::Private::addJob(Job* job) {
    STRIGI_MUTEX_LOCK(&mutex);

    // check if we can merge this job with a job from the waiting queue
    list<Job*>::iterator i, end = jobs.end();
    for (i = jobs.begin(); i != end; ++i) {
        if ((*i)->merge(job)) {
            STRIGI_MUTEX_UNLOCK(&mutex);
            return true;
        }
        if (job > *i) {
            break;
        }
    }
    // insert in front of position i
    if (i == end) {
        jobs.push_back(job);
    } else {
        jobs.insert(i, job);
    }
    // signal a couple of times to make sure
    pthread_cond_signal(&cond);
    STRIGI_MUTEX_UNLOCK(&mutex);
    return true;
}
void
JobQueue::nudge() {
    if (p) p->nudge();
}
void
JobQueue::Private::nudge() { 
    STRIGI_MUTEX_LOCK(&mutex);
    pthread_cond_signal(&cond);
    STRIGI_MUTEX_UNLOCK(&mutex);
}
Job*
JobQueue::Private::getNextJob() {
    Job* j = 0;
    STRIGI_MUTEX_LOCK(&mutex);
    if (keeprunning && jobs.size() == 0) {
        if (pthread_cond_wait(&cond, &mutex)) {
            cerr <<  "Error in cond_wait: " << strerror(errno) << endl;
        }
    }
    if (jobs.size()) {
        j = jobs.front();
        jobs.erase(jobs.begin());
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
    return j;
}
