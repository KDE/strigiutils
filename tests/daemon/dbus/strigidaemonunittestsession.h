#ifndef STRIGIDAEMONUNITTESTSESSION_H
#define STRIGIDAEMONUNITTESTSESSION_H

#include <string>

class StrigiDaemonUnitTestSession {
private:
    class Private;
    Private* const p;
public:
    StrigiDaemonUnitTestSession();
    ~StrigiDaemonUnitTestSession();
    void setStrigiDaemon(const char* strigidaemon);
    void setStrigiDir(const char* dir);
    void setDataDir(const char* dir);
    void setXDGDir(const char* xdgdir);
    void addBackend(const char* name, const char* plugindir);
    void addFile(const char* name, const std::string& content);
    void addFile(const char* name, const char* content, int contentlength);
    void start();
    void stop();
};

#endif
