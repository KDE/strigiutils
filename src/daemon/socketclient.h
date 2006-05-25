#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "clientinterface.h"

class SocketClient : public ClientInterface {
private:
    std::string socketpath;
    std::string error;
    std::vector<std::string> request;
    std::vector<std::string> response;

    int open();
    bool readResponse(int sd);
    bool sendRequest(int sd);
public:
    const std::string& getError() const { return error; }
    void setSocketName(const std::string& socketname);
    std::vector<std::string> query(const std::string &query);
    std::map<std::string, std::string> getStatus();
};

#endif
