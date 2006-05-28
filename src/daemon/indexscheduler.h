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
