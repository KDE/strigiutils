#include "socketserver.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
using namespace std;

void
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
        return;
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
        return;
    }

    if (::listen(sd, 5) < 0) {
        perror("cannot listen to port");
        return;
    }

    int n = 0;
    while (n++ < 10000) {
        addlen = sizeof(work);
        newSd = accept(sd, (struct sockaddr*)&(work), &addlen);
        if (newSd < 0) {
            perror("cannot accept connection ");
            return;
        }

        readRequest(newSd);

        /* the server is now free to accept another socket request */
    }
    if (close(sd) < 0) {
        perror("close socket");
    }
}
void
SocketServer::readRequest(int sd) {
    string request;
    char buf[1001];
    while (sd >= 0) {
        int r = recv(sd, buf, 1000, 0);
        printf("receive %i\n", r);
        if (r < 0) {
            error = "Error reading from socket: ";
            error += strerror(errno);
            closeConnection(sd);
            return;
        } else if (r == 0 || buf[r-1] == 0) {
            buf[r] = '\0';
            request += buf;
            closeConnection(sd);
        } else {
            buf[r] = '\0';
            request += buf;
        }
    }
}
void
SocketServer::closeConnection(int sd) {
}
