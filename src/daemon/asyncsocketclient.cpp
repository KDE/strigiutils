#include "asyncsocketclient.h"
#include <sstream>
using namespace std;

bool
AsyncSocketClient::statusChanged() {
    if (method.length() == 0 || !socket.statusChanged()) {
        return false;
    }
    if (method == "countHits") {
        handleCountHitsResponse();
    } else if (method == "query") {
        handleQueryResponse();
    }
    method.clear();
    return true;
}
bool
AsyncSocketClient::countHits(const std::string& query) {
    method = "countHits";
    string msg = method+"\n"+query+"\n\n";
    return socket.sendRequest(msg);
}
void
AsyncSocketClient::handleCountHitsResponse() {
    if (socket.getStatus() == AsyncSocket::Error) {
        hitcount = -1;
        return;
    }
    istringstream i(socket.getResponse());
    i >> hitcount;
}
bool
AsyncSocketClient::query(const std::string& query) {
    method = "query";
    string msg = method+"\n"+query+"\n\n";
    return socket.sendRequest(msg);
}
void
AsyncSocketClient::handleQueryResponse() {
    if (socket.getStatus() == AsyncSocket::Error) {
        hits.hits.clear();
    }
}
