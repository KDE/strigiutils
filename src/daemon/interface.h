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
    class IndexReader;
}

class Interface {
private:
    jstreams::IndexReader* reader;
public:
    Interface(jstreams::IndexReader* r) :reader(r) {}
    std::vector<std::string> query(const std::string& query);
};

#endif
