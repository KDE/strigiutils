#ifndef KMPSEARCHER_H
#define KMPSEARCHER_H

#include <string>

namespace jstreams {
/**
 * Class for string search that uses the Knuth-Morris-Pratt algorithm.
 * Code based on the example on
 * http://en.wikipedia.org/wiki/Knuth-Morris-Pratt_algorithm
 **/
class KMPSearcher {
private:
    std::string query;
    int32_t* table;
    int32_t len;
    int32_t maxlen;
public:
    KMPSearcher() :table(0) { }
    ~KMPSearcher() {
        if (table) {
            free(table);
        }
    }
    void setQuery(const std::string& );
    int32_t getQueryLength() const { return len; }
    std::string getQuery() const { return query; }
    const char* search(const char* haystack, int32_t haylen) const;
};
}

#endif
