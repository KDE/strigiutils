/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef REGISTEREDFIELD_H
#define REGISTEREDFIELD_H

#include "jstreamsconfig.h"
#include "cnstr.h"

namespace Strigi {

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
    const RegisteredField* getParent() const { return parent; }
    int getMaxOccurs() const { return maxoccurs; }
    const cnstr& getType() const { return type; }
};

class STREAMANALYZER_EXPORT FieldRegister {
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
