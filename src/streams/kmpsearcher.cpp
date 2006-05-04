#include "kmpsearcher.h"
using namespace jstreams;
using namespace std;

void
KMPSearcher::setQuery(const string& query) {
    this->query = query;
    len = query.length();
    const char* p = query.c_str();
    if (table) {
        if (len > maxlen) {
            table = (int32_t*)realloc(table, sizeof(int32_t)*(len+1));
            maxlen = len;
        }
    } else {
        table = (int32_t*)malloc(sizeof(int32_t)*(len+1));
        maxlen = len;
    }
    int32_t i = 0;
    int32_t j = -1;
    char c = '\0';

    // build the mismatch table    
    table[0] = j;
    while (i < len) {
        if (p[i] == c) {
            table[i + 1] = j + 1;
            ++j;
            ++i;
        } else if (j > 0) {
            j = table[j];
        } else {
            table[i + 1] = 0;
            ++i;
            j = 0;
        }
        c = p[j];
    }
}
const char*
KMPSearcher::search(const char* haystack, int32_t haylen) const {
    if (table == 0) return 0;
    printf("start search %i\n", len);
    const char* needle = query.c_str();
    // search for the pattern
    int32_t i = 0;
    int32_t j = 0;
    while (j + i < haylen && i < len) {
        if (haystack[j + i] == needle[i]) {
            ++i;
        } else {
            j += i - table[i];
            if (i > 0) i = table[i];
        }
    }
    
    if (needle[i] == '\0') {
        return haystack + j;
    } else {
        return 0;
    }
}

