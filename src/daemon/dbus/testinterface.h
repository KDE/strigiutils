#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

#include <string>
#include <vector>
#include <map>

class TestInterface {
public:
    TestInterface() {}
    std::string helloWorld(const std::string& name);
    bool upload(const std::vector<char>& upload);
    std::map<std::string, std::string> giveMap();
};

#endif
