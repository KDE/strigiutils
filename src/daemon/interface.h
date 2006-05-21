#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include <string>

/**
 * This class exposes the daemon functionality to the clients and should be
 * used by the client interfaces. The client interfaces should implement all
 * functions provided here. 
 **/

namespace jstreams {
    class IndexManager;
}

class Interface {
private:
    jstreams::IndexManager* manager;
public:
    Interface(jstreams::IndexManager* m) :manager(m) {}
    std::vector<std::string> query(const std::string& query);
};

#endif
