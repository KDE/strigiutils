#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include <string>
#include <vector>
#include <map>

class ClientInterface {
protected:
    bool active;
public:
    ClientInterface() :active(true) {}
    virtual ~ClientInterface() {}
    virtual std::vector<std::string> query(const std::string& query) = 0;
    virtual std::map<std::string, std::string> getStatus() = 0;
    virtual std::string stopDaemon() = 0;
    virtual std::string startIndexing() = 0;
    virtual std::string stopIndexing() = 0;
    bool isActive() { return active; }
};

#endif
