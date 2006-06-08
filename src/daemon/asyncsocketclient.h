#ifndef ASYNCSOCKETCLIENT_H
#define ASYNCSOCKETCLIENT_H

#include "asyncsocket.h"
#include "clientinterface.h"

class AsyncSocketClient {
private:
    AsyncSocket socket;
    std::string method;
    int hitcount;
    ClientInterface::Hits hits;

    void handleQueryResponse();
    void handleCountHitsResponse();
public:
    void setSocketPath(const std::string& path) {
        socket.setSocketPath(path);
    }
    bool countHits(const std::string& query);
    int getHitCount() const { return hitcount; }
    bool query(const std::string& query);
    const ClientInterface::Hits& getHits() const {
        return hits;
    }
    bool statusChanged();
};

#endif
