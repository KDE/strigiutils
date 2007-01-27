#ifndef TAGMAPPING_H
#define TAGMAPPPING_H

#include <map>
#include <string>

class TagMapping {
private:
    std::map<std::string, std::string> namespaces;
    std::map<std::string, std::string> mapping;
public:
    TagMapping(const char* mappingfile);
    const std::map<std::string, std::string>& getNamespaces() const {
        return namespaces;
    }
    const std::string& map(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator i
            = mapping.find(key);
        return (i == mapping.end()) ?key :i->second;
    }
};

#endif
