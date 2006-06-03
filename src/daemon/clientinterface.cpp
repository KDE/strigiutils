#include "clientinterface.h"

std::vector<std::string>
ClientInterface::getBackEnds() {
    std::vector<std::string> backends;
#ifdef HAVE_ESTRAIER
    backends.push_back("estraier");
#endif
#ifdef HAVE_CLUCENE
    backends.push_back("clucene");
#endif
#ifdef HAVE_XAPIAN
    backends.push_back("xapian");
#endif
#ifdef HAVE_SQLITE
    backends.push_back("sqlite");
#endif
    return backends;
}
