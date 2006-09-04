#ifndef STRIGITHREAD_H
#define STRIGITHREAD_H

#include <pthread.h>
#include <string>

class StrigiThread {
friend void* threadstarter(void *);
public:
    enum State {Idling, Working, Stopping};
private:
    State state;
protected:
    pthread_mutex_t lock;
    pthread_t thread;
    void setState(State s);
    virtual void* run(void*) = 0;
public:
    const char* const name;

    StrigiThread(const char* name);
    virtual ~StrigiThread();
    int start();
    void stop();
    void terminate();
    State getState();
    std::string getStringState();
};

#endif

