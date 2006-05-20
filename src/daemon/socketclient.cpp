#include "socketclient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

void
SocketClient::setSocketName(const std::string& n) {
    socketpath = n;
}
const std::vector<std::string>&
SocketClient::query(const std::string &query) {
    response.clear();
    request.clear();
    request.push_back("query");
    assert(query.find("\n") == std::string::npos);
    request.push_back(query);
    int sd = open();
    if (sd < 0) {
        printf("   %s\n", error.c_str());
        return response;
    }
    sendRequest(sd);
    readResponse(sd);
    return response;
}
int
SocketClient::open() {
    struct sockaddr_un  serv_addr;

    // create a socket
    int sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd < 0) {
        error = "Could not create socket: ";
        error += strerror(errno);
        return -1;
    }

    // set the address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    uint len = socketpath.length();
    len = (len > sizeof(serv_addr.sun_path)) ?sizeof(serv_addr.sun_path) :len;
    strncpy(serv_addr.sun_path, socketpath.c_str(), len);
    serv_addr.sun_path[len] = '\0';

    // connect to the server
    int r = connect(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (r < 0) {
        error = "Could not connect to server: ";
        error += strerror(errno);
        close(sd);
        return -1;
    }
    return sd;
}
bool
SocketClient::readResponse(int sd) {
    response.clear();
    std::string line;
    char c;
    while (true) {
        int r = recv(sd, &c, 1, 0);
        if (r < 0) {
            error = "Error reading from socket: ";
            error += strerror(errno);
            printf("%s\n", error.c_str());
            return false;
        } else if (r == 0 || c == 0) {
            if (line.size() > 0) {
                response.push_back(line);
            }
            return true;
        } else if (c == '\n') {
            printf("%s\n", line.c_str());
            if (line.size() == 0) {
                // finished reading the request
                return true;
            }
            response.push_back(line);
            line.clear();
        } else {
            line += c;
        }
    }
}
bool
SocketClient::sendRequest(int sd) {
    ssize_t r;
    for (uint i=0; i<request.size(); ++i) {
        std::string line = request[i];
        assert(line.find('\n') == std::string::npos);
        line += '\n';
        int p = 0;
        int len = line.length();
        do {
            r = send(sd, line.c_str()+p, len-p, MSG_NOSIGNAL);
            if (r < 0) {
                printf("error writing request\n");
                return false;
            }
            p += r;
        } while (p < len);
    }
    r = send(sd, "\n", 1, MSG_NOSIGNAL);
    return r > 0;
}
