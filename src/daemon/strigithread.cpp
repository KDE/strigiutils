#include "strigithread.h"
#include "strigilogging.h"
#include <string>
#include <cstring>
#include <errno.h>
#include <sys/resource.h>
using namespace std;

void*
threadstarter(void *d) {
    // give this thread job batch job priority
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = 0;
    StrigiThread* thread = static_cast<StrigiThread*>(d);
#ifndef SCHED_BATCH
#define SCHED_BATCH 3
#endif
    int r = sched_setscheduler(0, SCHED_BATCH, &param);
    if (r != 0) {
        // fall back to renice if SCHED_BATCH is unknown
        r = setpriority(PRIO_PROCESS, 0, 20);
        if (r==-1)
            STRIGI_LOG_ERROR (string("strigi.daemon.") + thread->name + ".threadstartert",
                string("error setting priority: ") + strerror(errno))
        //nice(20);
    }
#ifdef HAVE_LINUXIOPRIO
    sys_ioprio_set(IOPRIO_WHO_PROCESS, 0, IOPRIO_CLASS_IDLE);
#endif

    // start the actual work
    thread->run(0);
    STRIGI_LOG_DEBUG(string("strigi.daemon.") + thread->name + ".threadstartert", "end of thread");
    pthread_exit(0);
}

StrigiThread::StrigiThread(const char* n) :state(Idling),thread(0),  name(n) {
    pthread_mutex_init(&lock, NULL);
}
StrigiThread::~StrigiThread() {
    pthread_mutex_destroy(&lock);
}
void
StrigiThread::setState(State s) {
    pthread_mutex_lock(&lock);
    state = s;
    pthread_mutex_unlock(&lock);
}
StrigiThread::State
StrigiThread::getState() {
    State s;
    pthread_mutex_lock(&lock);
    s = state;
    pthread_mutex_unlock(&lock);
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
StrigiThread::start() {
    // start the indexer thread
    int r = pthread_create(&thread, NULL, threadstarter, this);
    if (r < 0) {
        STRIGI_LOG_ERROR ("strigi.daemon." + string(name), "cannot create thread")
        return 1;
    }
    return 0;
}
void
StrigiThread::stop() {
    state = Stopping;
    if (thread) {
        // wait for the thread to finish
        pthread_join(thread, 0);
    }
    thread = 0;
}
void
StrigiThread::terminate() {
    // TODO
}
