#ifndef REGISTEREDFIELD_H
#define REGISTEREDFIELD_H

#include "cnstr.h"

namespace jstreams {

class RegisteredField {
friend class FieldRegister;
private:
    const cnstr key;
    const cnstr type;
    const RegisteredField* parent;
    void* writerdata;
    const int maxoccurs;

    RegisteredField();
    RegisteredField(const cnstr& key, const cnstr& type, int maxoccurs,
        const RegisteredField* parent);
public:
    const cnstr& getKey() const { return key; }
    void* getWriterData() const { return writerdata; }
    void setWriterData(void* d) { writerdata = d; }
};

class FieldRegister {
private:
    std::map<cnstr, const RegisteredField*> fields;
public:
    FieldRegister() {}
    ~FieldRegister();
    const RegisteredField* registerField(const cnstr& fieldname,
        const cnstr& type, int maxoccurs, const RegisteredField* parent);

    static cnstr floatType;
    static cnstr integerType;
    static cnstr binaryType;
    static cnstr stringType;

    const RegisteredField* pathField;
    const RegisteredField* encodingField;
    const RegisteredField* mimetypeField;
    const RegisteredField* filenameField;
    const RegisteredField* extensionField;
    const RegisteredField* embeddepthField;
    const RegisteredField* mtimeField;
    const RegisteredField* sizeField;
};

}

#endif
