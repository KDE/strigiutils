#ifndef STRIGIDAEMONUNITTESTSESSION_H
#define STRIGIDAEMONUNITTESTSESSION_H

class StrigiDaemonUnitTestSession {
private:
    class Private;
    Private* const p;
public:
    StrigiDaemonUnitTestSession();
    ~StrigiDaemonUnitTestSession();
    void setStrigiDaemon(const char* strigidaemon);
    void setXDGDir(const char* xdgdir);
    void addBackend(const char* name, const char* plugindir);
    void start();
    void stop();
};

#endif
