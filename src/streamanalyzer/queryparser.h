#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "query.h"

namespace Strigi {

class STREAMANALYZER_EXPORT QueryParser {
public:
    static Query buildQuery(const std::string& query);
};

}

#endif
