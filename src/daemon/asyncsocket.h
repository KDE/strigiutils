#ifndef ASYNCSOCKET_H
#define ASYNCSOCKET_H

#include <string>

class AsyncSocket {
public:
    enum Status { Idle, Writing, Reading, Error };
private:
    Status status;
    int socket;
    uint writepos;
    std::string socketpath;
    std::string error;
    std::string request;
    std::string response;

    void open();
    void close();
    void read();
    void write();
public:
    AsyncSocket();
    void setSocketPath(const std::string& path) {
        socketpath = path;
    }
    ~AsyncSocket();
    bool statusChanged();
    bool sendRequest(const std::string& req);
    const std::string& getResponse() const { return response; }
    Status getStatus() const { return status; }
    const std::string& getError() const { return error; }
};

#endif
