#include "fieldtypes.h"
using namespace jstreams;
using namespace std;

RegisteredField::RegisteredField(const cnstr& k, const cnstr& t, int m,
        const RegisteredField* p)
        : key(k), type(t), maxoccurs(m), parent(p) {
}

cnstr FieldRegister::floatType = "float";
cnstr FieldRegister::integerType = "integer";
cnstr FieldRegister::binaryType = "binary";
cnstr FieldRegister::stringType = "string";

FieldRegister::~FieldRegister() {
    map<cnstr, const RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        delete i->second;
    }
}
const RegisteredField*
FieldRegister::registerField(const cnstr& fieldname,
        const cnstr& type, int maxoccurs, const RegisteredField* parent) {
    map<cnstr, const RegisteredField*>::iterator i = fields.find(fieldname);
    if (i == fields.end()) {
        const RegisteredField* f = new RegisteredField(fieldname, type,
            maxoccurs, parent);
        fields[fieldname] = f;
        return f;
    } else {
        // check that the field being registered is the same as the one that
        // has already been registered
    }
    return i->second;
}
