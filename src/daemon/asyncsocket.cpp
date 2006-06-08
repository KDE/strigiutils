#include "asyncsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

AsyncSocket::AsyncSocket() {
    status = Idle;
}
AsyncSocket::~AsyncSocket() {
    close();
}
void
AsyncSocket::open() {
    struct sockaddr_un  serv_addr;

    // create a socket
    socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket < 0) {
        error = "Could not create socket: ";
        error += strerror(errno);
        return;
    }

    // set the address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    uint len = socketpath.length();
    len = (len > sizeof(serv_addr.sun_path)) ?sizeof(serv_addr.sun_path) :len;
    strncpy(serv_addr.sun_path, socketpath.c_str(), len);
    serv_addr.sun_path[len] = '\0';

    // connect to the server
    int r = connect(socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (r < 0) {
        error = "Could not connect to server: ";
        error += strerror(errno);
        ::close(socket);
        socket = -1;
    }
}
void
AsyncSocket::close() {
    if (socket >= 0) {
        ::close(socket);
    }
}
bool
AsyncSocket::sendRequest(const std::string& req) {
    if (status == Reading || status == Writing) {
        close();
    }
    open();
    if (socket < 0) {
        status = Error;
        return false;
    }
    error.clear();
    request = req;
    response.clear();
    writepos = 0;
    status = Writing;
    return true;
}
bool
AsyncSocket::statusChanged() {
    if (status == Error || status == Idle) return false;
    if (status == Writing) {
        write();
    }
    if (status == Reading) {
        read();
    }
    return status == Error || status == Idle;
}
void
AsyncSocket::write() {
    ssize_t r = send(socket, request.c_str()+writepos,
        request.length()-writepos, MSG_NOSIGNAL);
    if (r != -1) {
        writepos += r;
        if (writepos == request.length()) {
            status = Reading;
        }
    } else {
        switch (errno) {
        case EAGAIN:
            break;
        default:
            status = Error;
            error = strerror(errno);
            break;
        }
    }
}
void
AsyncSocket::read() {
    char c;
    while (true) {
        int r = recv(socket, &c, 1, MSG_DONTWAIT|MSG_NOSIGNAL);
        switch (r) {
        case 0:
            close();
            status = Idle;
            return;
        case 1:
            response += c;
            break;
        default:
            if (errno != EAGAIN) {
                printf("hmm %i\n", errno);
                status = Error;
                error = strerror(EAGAIN);
                close();
            }
            return;
        }
    }
}
