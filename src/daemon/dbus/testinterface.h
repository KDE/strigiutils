#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

#include <string>
#include <vector>
#include <map>

class TestInterface {
public:
    TestInterface() {}
    std::string helloWorld(const std::string& name);
    std::string concat(const std::string& a, const std::string& b);
    bool upload(const std::vector<char>& upload);
    std::map<std::string, std::string> giveMap();
};

#endif
