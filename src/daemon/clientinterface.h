#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include "indexeddocument.h"
#include <vector>

class ClientInterface {
protected:
    bool active;
public:
/*    class Hit {
    public:
        std::string uri;
        double score;
        std::string fragment;
        std::string mimetype;
        int64_t size;
        time_t mtime;
        std::map<std::string, std::string> properties;
    };*/
    class Hits {
    public:
        std::vector<jstreams::IndexedDocument> hits;
        std::string error;
    };

    ClientInterface() :active(true) {}
    virtual ~ClientInterface() {}
    virtual int countHits(const std::string& query) = 0;
    virtual Hits query(const std::string& query) = 0;
    virtual std::map<std::string, std::string> getStatus() = 0;
    virtual std::string stopDaemon() = 0;
    virtual std::string startIndexing() = 0;
    virtual std::string stopIndexing() = 0;
    virtual std::vector<std::string> getIndexedDirectories() = 0;
    virtual std::string setIndexedDirectories(std::vector<std::string>) = 0;
    bool isActive() { return active; }
    static std::vector<std::string> getBackEnds();

};

#endif
