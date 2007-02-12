#ifndef REGISTEREDFIELD_H
#define REGISTEREDFIELD_H

#include "cnstr.h"

namespace jstreams {

class RegisteredField {
friend class FieldRegister;
private:
    const cnstr key;
    const cnstr type;
    const int maxoccurs;
    const RegisteredField* parent;
    void* writerdata;

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
    std::map<cnstr, RegisteredField*> fields;
public:
    FieldRegister();
    ~FieldRegister();
    const RegisteredField* registerField(const cnstr& fieldname,
        const cnstr& type, int maxoccurs, const RegisteredField* parent);
    const std::map<cnstr, RegisteredField*>& getFields() const {
        return fields;
    }
    std::map<cnstr, RegisteredField*>& getFields() {
        return fields;
    }

    static const cnstr floatType;
    static const cnstr integerType;
    static const cnstr binaryType;
    static const cnstr stringType;

    static const cnstr pathFieldName;
    static const cnstr encodingFieldName;
    static const cnstr mimetypeFieldName;
    static const cnstr filenameFieldName;
    static const cnstr extensionFieldName;
    static const cnstr embeddepthFieldName;
    static const cnstr mtimeFieldName;
    static const cnstr sizeFieldName;

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
