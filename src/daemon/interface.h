#ifndef INTERFACE_H
#define INTERFACE_H

#include "clientinterface.h"

/**
 * This class exposes the daemon functionality to the clients and should be
 * used by the client interfaces. The client interfaces should implement all
 * functions provided here. 
 **/

namespace jstreams {
    class IndexManager;
}
class IndexScheduler;
class Interface : public ClientInterface {
private:
    jstreams::IndexManager& manager;
    IndexScheduler& scheduler;
public:
    Interface(jstreams::IndexManager& m, IndexScheduler& s) :manager(m),
        scheduler(s) {}
    Hits query(const std::string& query);
    std::map<std::string, std::string> getStatus();
    std::string stopDaemon();
    std::string startIndexing();
    std::string stopIndexing();
    std::vector<std::string> getIndexedDirectories();
    std::string setIndexedDirectories(std::vector<std::string>);
};

#endif
