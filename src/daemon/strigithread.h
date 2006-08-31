#ifndef STRIGITHREAD_H
#define STRIGITHREAD_H

#include <pthread.h>
class StrigiThread {
friend void* threadstarter(void *);
protected:
    enum State {Idling, Working, Stopping};
    State state;
    pthread_mutex_t lock;
    static pthread_mutex_t initlock;
    pthread_t thread;
    virtual void* run(void*) = 0;
public:
    StrigiThread();
    virtual ~StrigiThread() {}
    int start();
    void stop();
    void terminate();
};

#endif

