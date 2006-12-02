#include <sys/inotify.h>
#include <list>
#include <string>

class INotifyWatcher {
private:
    int m_iEvents;
    int m_inotifyfd;
    std::list<std::string> dirs;

    void bootstrapDir(const std::string& dir);

    static void indexFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime);
    static void watchDirCallback(const char* path, uint len);
public:
    INotifyWatcher();
    void addDir(const std::string& dir);
};

#include "filelister.h"
#include "indexerconfiguration.h"
using namespace std;
using namespace jstreams;

INotifyWatcher::INotifyWatcher() {
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
        | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;
    m_inotifyfd = inotify_init();
    if (m_inotifyfd < 0) {
        fprintf(stderr, "INotify cannot be initialized.\n");
    }
}
void
INotifyWatcher::addDir(const std::string& dir) {
    IndexerConfiguration ic;
    FileLister lister(ic);
    lister.setFileCallbackFunction(&indexFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);
}
void
INotifyWatcher::indexFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime) {
}
void
INotifyWatcher::watchDirCallback(const char* path, uint len) {
}

int
main(int argc, char** argv) {
    INotifyWatcher watcher;
    return 0;
}
