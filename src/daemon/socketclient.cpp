#include "socketclient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <assert.h>
#include <sstream>
using namespace std;

void
SocketClient::setSocketName(const std::string& n) {
    socketpath = n;
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
int
SocketClient::countHits(const std::string &query) {
    response.clear();
    request.clear();
    request.push_back("countHits");
    assert(query.find("\n") == std::string::npos);
    request.push_back(query);
    int sd = open();
    if (sd < 0) {
        fprintf(stderr, "   %s\n", error.c_str());
        return -1;
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    if (response.size() == 0) return -1;
    int count = atoi(response[0].c_str());
    return count;
}
ClientInterface::Hits
SocketClient::getHits(const std::string &query, int max, int off) {
    response.clear();
    request.clear();
    request.push_back("query");
    assert(query.find("\n") == std::string::npos);
    request.push_back(query);
    ostringstream oss;
    oss << max;
    request.push_back(oss.str());
    oss.str("");
    oss << off;
    request.push_back(oss.str());
    int sd = open();
    Hits hits;
    if (sd < 0) {
        printf("   %s\n", error.c_str());
        hits.error = error;
        return hits;
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
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
            if (d && d < v) {
                break;
            }
            string n(s, v-s);
            h.properties[n] = v+1;
            ++i;
        }
        hits.hits.push_back(h);
    }
    response.clear();
    return hits;
}
map<string, string>
SocketClient::getStatus() {
    map<string, string> status;
    response.clear();
    request.clear();
    request.push_back("getStatus");
    int sd = open();
    if (sd < 0) {
        // no connection: return an empty map
        //printf("   %s\n", error.c_str());
        //status["error"] = error;
        return status;
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    for (uint i=0; i<response.size(); ++i) {
        string s = response[i];
        uint p = s.find(":");
        if (p == string::npos) {
            printf("''%s''\n", s.c_str());
            status.clear();
            status["error"] = "Communication error.";
            return status;
        }
        status[s.substr(0,p)] = s.substr(p+1);
    }
    return status;
}
std::string
SocketClient::stopDaemon() {
    request.clear();
    request.push_back("stopDaemon");
    int sd = open();
    if (sd < 0) {
        return "";
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    return "";
}
std::string
SocketClient::startIndexing() {
    request.clear();
    request.push_back("startIndexing");
    int sd = open();
    if (sd < 0) {
        return "";
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    return "";
}
std::string
SocketClient::stopIndexing() {
    request.clear();
    request.push_back("stopIndexing");
    int sd = open();
    if (sd < 0) {
        return "";
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    return "";
}
std::vector<std::string>
SocketClient::getIndexedDirectories() {
    request.clear();
    response .clear();
    request.push_back("getIndexedDirectories");
    int sd = open();
    if (sd < 0) {
        return response;
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    return response;
}
std::string
SocketClient::setIndexedDirectories(std::vector<std::string> dirs) {
    request.clear();
    request.push_back("setIndexedDirectories");
    for (uint i=0; i<dirs.size(); ++i) {
        request.push_back(dirs[i]);
    }
    int sd = open();
    if (sd < 0) {
        return "";
    }
    sendRequest(sd);
    readResponse(sd);
    close(sd);
    return "";
}
