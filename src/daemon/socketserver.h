#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "server.h"
#include <string>

class SocketServer : public Server {
private:
    std::string socketname;
    std::string error;

    void readRequest(int sd);
    void closeConnection(int sd);
protected:
    void listen();
};

#endif
