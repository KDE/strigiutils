#ifndef INDEXSCHEDULER_H
#define INDEXSCHEDULER_H

#include <map>
#include <string>
#include <pthread.h>

namespace jstreams {
    class IndexManager;
}
class IndexScheduler {
friend void* indexschedulerstart(void *);
friend bool addFileCallback(const std::string& path, const char *filename,
    time_t mtime);
private:
    pthread_mutex_t lock;
    static pthread_mutex_t initlock;
    bool keeprunning;
    pthread_t thread;
    std::string dirtoindex;
    jstreams::IndexManager* indexmanager;
    std::map<std::string, time_t> dbfiles;
    std::map<std::string, time_t> toindex;

    void* run(void*);
public:
    static bool addFileCallback(const std::string& path, const char *filename,
        time_t mtime);
    IndexScheduler();
    void setDirToIndex(const std::string& d) {
        dirtoindex = d;
    }
    void setIndexManager(jstreams::IndexManager* m) {
        indexmanager = m;
    }
    int getQueueSize();
    int start();
    void stop();
    void terminate();
    ~IndexScheduler();
};

#endif
