/* a client in the unix domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>

class Connection {
private:
    struct sockaddr_un  serv_addr;
    int sd;
    std::string socketpath;
    std::string error;
    std::string response;

    bool open();
    void close_();
public:
    Connection(const char* socketname);
    ~Connection();
    bool sendMessage(const std::string& msg);
    const std::string& getResponse() const { return response; }
    const std::string& getError() const { return error; }
};
Connection::Connection(const char* socketname) {
    socketpath = socketname;
    sd = -1;
    error = "";
}
Connection::~Connection() {
    close_();
}
bool
Connection::open() {
    // if another connection is still open, close it
    close_();

    // create a socket
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd < 0) {
        error = "Could not create socket: ";
        error += strerror(errno);
        return false;
    }

    // set the address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    int len = socketpath.length();
    len = (len > sizeof(serv_addr.sun_path)) ?sizeof(serv_addr.sun_path) :len;
    strncpy(serv_addr.sun_path, socketpath.c_str(), len);
    serv_addr.sun_path[len] = '\0';

    // connect to the server
    int r = connect(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (r < 0) {
        error = "Could not connect to server: ";
        error += strerror(errno);
        close_();
        return false;
    }
    return true;
}
bool
Connection::sendMessage(const std::string& msg) {
    if (!open()) return false;
    response = "";
    // send first
/*    int pos = 0;
    int len = msg.length()+1; //include '\0'
    while (sd >= 0 && pos < len) {
        int r = send(sd, msg.c_str()+pos, len-pos, MSG_NOSIGNAL);
        if (r < 0) {
            error = "Error writing to socket: ";
            error += strerror(errno);
            close_();
            return false;
        }
        pos += r;
    }
    printf("after sending\n");*/
    char buf[1001];
    while (sd >= 0) {
        int r = recv(sd, buf, 1000, 0);
        printf("receive %i\n", r);
        if (r < 0) {
            error = "Error reading from socket: ";
            error += strerror(errno);
            close_();
            return false;
        } else if (r == 0 || buf[r-1] == 0) {
            buf[r] = '\0';
            response += buf;
            close_();
        } else {
            buf[r] = '\0';
            response += buf;
        }
    }
    return true;
}
void
Connection::close_() {
    if (sd >= 0) {
        close(sd);
        sd = -1;
    }
}

int
main(int argc, char *argv[]) {
    if (argc <= 1) return 0;
    Connection c(argv[1]);
    if (c.sendMessage("hi")) {
        printf("%s\n", c.getResponse().c_str());
    } else {
        printf("!%s\n", c.getError().c_str());
    }
}

