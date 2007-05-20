#ifndef XESAMQUERY_H
#define XESAMQUERY_H

#include <vector>
#include <string>

/** these classes capture the query capabilities of Xesam **/
class TermPrivate;
class Term {
private:
    TermPrivate* const p;
public:
    enum Type { String, Integer, Date, Boolean, Float };

    Term();
    Term(const Term&);
    ~Term();
    void operator=(const Term&);

    bool caseSensitive() const;
    void setCaseSensitive(bool b);
    bool diacriticSensitive() const;
    void setDiacriticSensitive(bool b);
    bool stemming() const;
    void setStemming(bool b);
    float fuzzy() const;
    void setFuzzy(float f);
    int slack() const;
    void setSlack(int s);
    int proximityDistance() const;
    void setProximityDistance(int d);
    bool ordered() const;
    void setOrdered(bool b);
    bool wordBased() const;
    void setWordBased(bool b);

    void setValue(const std::string& s);
    void setValue(const char* s);
};

class QueryPrivate;
class Query {
private:
    QueryPrivate* const p;
public:
    enum Type { And, Or,
        Equals, Contains, LessThan, LessThanEquals,
        GreaterThan, GreaterThanEquals, StartsWith, FullText, RegExp,
        Proximity };

    Query();
    Query(const Query& q);
    ~Query();
    void operator=(const Query&);

    Term& term();
    const Term& term() const;
    void setTerm(const Term& t);
    float boost() const;
    void setBoost(int i);
    Type type() const;
    void setType(Type t);
    bool negate() const;
    void setNegate(bool n);
    std::vector<std::string>& fields();
    const std::vector<std::string>& fields() const;
    std::vector<Query>& subQueries();
    const std::vector<Query>& subQueries() const;
};

#endif
