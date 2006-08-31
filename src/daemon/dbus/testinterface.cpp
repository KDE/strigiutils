#include "testinterface.h"
using namespace std;

std::string
TestInterface::helloWorld(const std::string& name) {
    return "Hello "+name;
}
std::string
TestInterface::concat(const std::string& a, const std::string& b) {
    return a+b;
}
bool
TestInterface::upload(const std::vector<char>& upload) {
    printf("got file of size %i uploaded\n", upload.size());
    return true;
}
map<string, string>
TestInterface::giveMap() {
    map<string, string> m;
    m["a"] = "b";
    return m;
}
