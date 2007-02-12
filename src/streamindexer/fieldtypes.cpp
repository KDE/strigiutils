#include "fieldtypes.h"
using namespace jstreams;
using namespace std;

RegisteredField::RegisteredField(const cnstr& k, const cnstr& t, int m,
        const RegisteredField* p)
        : key(k), type(t), maxoccurs(m), parent(p), writerdata(0) {
}

const cnstr FieldRegister::floatType = "float";
const cnstr FieldRegister::integerType = "integer";
const cnstr FieldRegister::binaryType = "binary";
const cnstr FieldRegister::stringType = "string";

const cnstr FieldRegister::pathFieldName = "path";
const cnstr FieldRegister::encodingFieldName = "encoding";
const cnstr FieldRegister::mimetypeFieldName = "mimetype";
const cnstr FieldRegister::filenameFieldName = "filename";
const cnstr FieldRegister::extensionFieldName = "ext";
const cnstr FieldRegister::embeddepthFieldName = "depth";
const cnstr FieldRegister::mtimeFieldName = "mtime";
const cnstr FieldRegister::sizeFieldName = "size";

FieldRegister::FieldRegister() {
    pathField = registerField(pathFieldName, stringType, 1, 0);
    encodingField = registerField(encodingFieldName, stringType, 1, 0);
    mimetypeField = registerField(mimetypeFieldName, stringType, -1, 0);
    filenameField = registerField(filenameFieldName, stringType, 1, 0);
    extensionField = registerField(extensionFieldName, stringType, 1, 0);
    embeddepthField = registerField(embeddepthFieldName, integerType, 1, 0);
    mtimeField = registerField(mtimeFieldName, integerType, 1, 0);
    sizeField = registerField(sizeFieldName, integerType, 1, 0);
}

FieldRegister::~FieldRegister() {
    map<cnstr, RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        delete i->second;
    }
}
const RegisteredField*
FieldRegister::registerField(const cnstr& fieldname,
        const cnstr& type, int maxoccurs, const RegisteredField* parent) {
    map<cnstr, RegisteredField*>::iterator i = fields.find(fieldname);
    if (i == fields.end()) {
        RegisteredField* f = new RegisteredField(fieldname, type,
            maxoccurs, parent);
        fields[fieldname] = f;
        return f;
    } else {
        // check that the field being registered is the same as the one that
        // has already been registered
    }
    return i->second;
}
