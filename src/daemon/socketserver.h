#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "server.h"
#include <string>
#include <vector>

class SocketServer : public Server {
private:
    std::string socketname;
    std::string error;
    std::vector<std::string> request;
    std::vector<std::string> response;

    bool readRequest(int sd);
    bool sendResponse(int sd);
    void handleRequest();
protected:
    void listen();
public:
    SocketServer(Interface* i) :Server(i) {}
    void setSocketName(const std::string& name) {
        socketname = name;
    }
};

#endif
