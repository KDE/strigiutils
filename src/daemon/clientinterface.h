#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include <string>
#include <vector>
#include <map>

class ClientInterface {
public:
    virtual ~ClientInterface() {}
    virtual std::vector<std::string> query(const std::string& query) = 0;
    virtual std::map<std::string, std::string> getStatus() = 0;
};

#endif
