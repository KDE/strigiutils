#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include <string>
#include <vector>

class ClientInterface {
public:
    virtual ~ClientInterface() {}
    virtual const std::vector<std::string>& query(const std::string& query) = 0;
};

#endif
