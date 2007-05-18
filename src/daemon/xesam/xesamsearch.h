#ifndef XESAMSEARCH_H
#define XESAMSEARCH_H

#include <string>
#include <vector>

class Variant;
class XesamSession;
class XesamSearch {
public:
    const std::string name;
    XesamSession& session;

    XesamSearch(XesamSession& s, const std::string& n,
        const std::string& query) :name(n), session(s) {}
    ~XesamSearch() { }
    int32_t countHits() { return 10; }
    std::vector<std::vector<Variant> > getHits(int32_t num);
    std::vector<std::vector<Variant> > getHitData(
        const std::vector<int32_t>& hit_ids,
        const std::vector<std::string>& properties);
};

#endif
