#ifndef XESAMQUERY_H
#define XESAMQUERY_H

#include <vector>
#include <string>

/** these classes capture the query capabilities of Xesam **/

class Term {
public:
    enum Type { String, Integer, Date, Boolean, Float };
    Type type;
    int64_t i_value;     // value for Integer, Date and Boolean
    double d_value;      // value for Float
    std::string s_value; // value for String

    // flags for String terms (ordered by size)
    std::string language;
    float fuzzy;
    int slack;
    bool phrase;
    bool caseSensitive;
    bool diacriticSensitive;
    bool ordered;
    bool enableStemming;
};

class Selection {
public:
    enum Type { Equals, Contains, LessThan, LessThanEquals, GreaterThan,
        GreaterThanEquals, StartsWith, FullText, RegExp, Proximity };
    Term first;
    Term second;
    Type type;
    std::vector<std::string> fields;
};

class Collectible {
public:
    Selection selection;
    std::vector<Collectible> collectibles;
    bool combineWithAnd;
    bool negate;
    float boost;
};

class XesamQuery {
public:
    std::vector<Collectible> collectibles;
    std::string type;
};

#endif
