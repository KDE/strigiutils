#include "xesamquery.h"
using namespace std;

class TermPrivate {
public:
    int64_t i_value;     // value for Integer, Date and Boolean
    double d_value;      // value for Float
    std::string s_value; // value for String

    // flags for String terms (ordered by size)
    std::string language;
    float fuzzy;
    int slack;
    int proximityDistance;
    Term::Type type;
    bool phrase;
    bool caseSensitive;
    bool diacriticSensitive;
    bool ordered;
    bool enableStemming;
    bool wordbased;
};
class QueryPrivate {
public:
    // Selection fields
    Term term;
    Term termTwo; // needed for Proximity
    std::vector<std::string> fields;
    // Collectible fields
    std::vector<Query> subs;
    // Shared fields
    float boost;
    bool negate;
    // Type
    Query::Type type;

    QueryPrivate() :boost(1), negate(false), type(Query::Contains) {}
};

Term::Term() :p(new TermPrivate()) {
}
Term::Term(const Term& t) :p(new TermPrivate(*t.p)) {
}
Term::~Term() {
    delete p;
}
void
Term::operator=(const Term& t) {
    *p = *t.p;
}
int
Term::proximityDistance() const {
    return p->proximityDistance;
}
void
Term::setProximityDistance(int d) {
    p->proximityDistance = d;
}
bool
Term::caseSensitive() const {
    return p->caseSensitive;
}
void
Term::setCaseSensitive(bool b) {
    p->caseSensitive = b;
}
bool
Term::diacriticSensitive() const {
    return p->diacriticSensitive;
}
void
Term::setDiacriticSensitive(bool b) {
    p->diacriticSensitive = b;
}
bool
Term::stemming() const {
    return p->enableStemming;
}
void
Term::setStemming(bool b) {
    p->enableStemming = b;
}
float
Term::fuzzy() const {
    return p->fuzzy;
}
void
Term::setFuzzy(float f) {
    p->fuzzy = f;
}
int
Term::slack() const {
    return p->slack;
}
void
Term::setSlack(int s) {
    p->slack = s;
}
bool
Term::ordered() const {
    return p->ordered;
}
void
Term::setOrdered(bool b) {
    p->ordered = b;
}
bool
Term::wordBased() const {
    return p->wordbased;
}
void
Term::setWordBased(bool b) {
    p->wordbased = b;
}
void
Term::setValue(const std::string& s) {
}
void
Term::setValue(const char* s) {
}
Query::Query() :p(new QueryPrivate()) {
}
Query::Query(const Query& t) :p(new QueryPrivate(*t.p)) {
}
Query::~Query() {
    delete p;
}
void
Query::operator=(const Query& q) {
    *p = *q.p;
}
Term&
Query::term() {
    return p->term;
}
const Term&
Query::term() const {
    return p->term;
}
float
Query::boost() const {
    return p->boost;
}
void
Query::setBoost(int b) {
    p->boost = b;
}
Query::Type
Query::type() const {
    return p->type;
}
void
Query::setType(Type t) {
    p->type = t;
}
bool
Query::negate() const {
    return p->negate;
}
void
Query::setNegate(bool n) {
    p->negate = n;
}
const vector<string>&
Query::fields() const {
    return p->fields;
}
vector<string>&
Query::fields() {
    return p->fields;
}
const vector<Query>&
Query::subQueries() const {
    return p->subs;
}
vector<Query>&
Query::subQueries() {
    return p->subs;
}
