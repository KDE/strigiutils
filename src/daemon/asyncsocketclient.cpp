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
vector<string>
AsyncSocketClient::splitResponse() const {
    vector<string> response;
    const char* p = socket.getResponse().c_str();
    const char* l = p;
    while (true) {
        while (*p != '\n' && *p != '\0') {p++;}
        if (p-l > 0) {
            string line(l, p-l);
            response.push_back(line);
            l = p+1;
        }
        if (*p == '\0') break;
        p++;
    }
    return response;
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
    hits.hits.clear();
    if (socket.getStatus() == AsyncSocket::Error) {
        return;
    }
    vector<string> response(splitResponse());
    uint i = 0;
    while (i+6 < response.size()) {
        jstreams::IndexedDocument h;
        h.uri = response[i++];
        h.fragment = response[i++];
        h.mimetype = response[i++];
        h.score = atof(response[i++].c_str());
        h.size = atoi(response[i++].c_str());
        h.mtime = atoi(response[i++].c_str());
        while (i < response.size()) {
            const char* s = response[i].c_str();
            const char* v = strchr(s, ':');
            if (!v) break;
            const char* d = strchr(s, '/');
            if (d && d < v) break;
            string n(s, v-s);
            h.properties[n] = v+1;
            ++i;
        }
        hits.hits.push_back(h);
    }
}
