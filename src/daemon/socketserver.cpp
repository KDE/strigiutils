#include "socketserver.h"
#include "interface.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <assert.h>
using namespace std;

bool
SocketServer::listen() {
    int sd, newSd;
    size_t len;
        unsigned int addlen;
    struct sockaddr_un sock, work;

    /* create new socket */
    unlink(socketname.c_str());
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("cannot open socket ");
        return false;
    }

    /* set the address */
    len = socketname.length()+1;
    len = (len > sizeof(sock.sun_path)) ?sizeof(sock.sun_path) :len;
    strncpy(sock.sun_path, socketname.c_str(), len);
    sock.sun_path[len] = '\0';

    /* bind server port */
    sock.sun_family = AF_UNIX;
    if (bind(sd, (struct sockaddr *)&sock, sizeof(sock))<0) {
        perror("cannot bind port ");
        return false;
    }

    if (::listen(sd, 5) < 0) {
        perror("cannot listen to port");
        return false;
    }

    while (interface->isActive()) {
        addlen = sizeof(work);
        newSd = accept(sd, (struct sockaddr*)&(work), &addlen);
        if (newSd < 0) {
            perror("cannot accept connection ");
            return false;
        }

        if (!readRequest(newSd)) {
            close(sd);
            continue;
        }
        response.clear();
        handleRequest();
        sendResponse(newSd);
        close(newSd);

        /* the server is now free to accept another socket request */
    }
    if (close(sd) < 0) {
        perror("close socket");
    }
    return true;
}
bool
SocketServer::readRequest(int sd) {
    request.clear();
    string line;
    char c;
    while (true) {
        int r = recv(sd, &c, 1, 0);
        if (r < 0) {
            error = "Error reading from socket: ";
            error += strerror(errno);
            return false;
        } else if (r == 0 || c == 0) {
            if (line.size() > 0) {
                request.push_back(line);
            }
            return true;
        } else if (c == '\n') {
            if (line.size() == 0) {
                // finished reading the request
                return true;
            }
            request.push_back(line);
            line.clear();
        } else {
            line += c;
        }
    }
    return true;
}
bool
SocketServer::sendResponse(int sd) {
    ssize_t r;
    for (uint i=0; i<response.size(); ++i) {
        string line = response[i];
        assert(line.find('\n') == string::npos);
        line += '\n';
        int p = 0;
        int len = line.length();
        do {
            r = send(sd, line.c_str()+p, len-p, MSG_NOSIGNAL);
            if (r < 0) {
                printf("error writing response\n");
                return false;
            }
            p += r;
        } while (p < len);
    }
    r = send(sd, "\n", 1, MSG_NOSIGNAL);
    return r > 0;
}
void
SocketServer::handleRequest() {
    response.clear();
    if (request.size() == 2 && request[0] == "query") {
        response = interface->query(request[1]);
        return;
    }
    if (request.size() == 1 && request[0] == "getStatus") {
        map<string, string> status = interface->getStatus();
        map<string,string>::const_iterator i;
        response.clear();
        for (i = status.begin(); i != status.end(); ++i) {
            response.push_back(i->first+":"+i->second);
        }
        return;
    }
    printf("request: '%s'\n", request[0].c_str());
    if (request.size() == 1 && request[0] == "stopDaemon") {
        interface->stopDaemon();
        return;
    }
    if (request.size() == 1 && request[0] == "startIndexing") {
        interface->startIndexing();
        return;
    }
    if (request.size() == 1 && request[0] == "stopIndexing") {
        interface->stopIndexing();
        return;
    }
    printf("size %i\n", request.size());
    response.push_back("error");
    response.push_back("no valid request");
}
