#ifndef XESAMSESSION_H
#define XESAMSESSION_H

#include <string>
#include <list>
#include "variant.h"

class XesamLiveSearch;
class XesamSearch;
class XesamSession {
private:
    std::list<const XesamSearch*> searches;
    XesamLiveSearch& xesam;
    bool searchLive;
    bool searchBlocking;
    std::vector<std::string> hitFields;
    std::vector<std::string> hitFieldsExtended;
    int32_t hitSnippetLength;
    std::string sortPrimary;
    std::string sortSecondary;
    bool sortAscending;
public:
    XesamSession(XesamLiveSearch& x);
    ~XesamSession();
    Variant setProperty(const std::string& prop, const Variant& v);
    Variant getProperty(const std::string& prop);
    const std::string& newSearch(const std::string& xml_query);
    void closeSearch(const XesamSearch* search);
};

#endif
