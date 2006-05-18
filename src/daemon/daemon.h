#ifndef DAEMON_H
#define DAEMON_H

class Daemon {
public:
    Daemon();
    void index(const std::string&);
    std::list<std::string> query(const std::string&);
};

class DaemonListener {
private:
    Daemon* daemon;
protected:
    DaemonListener(Daemon* d);
public:
    void start();
    void stop();
};

#endif
