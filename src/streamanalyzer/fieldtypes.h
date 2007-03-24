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
    const std::string key;
    const std::string type;
    const int maxoccurs;
    const RegisteredField* parent;
    void* writerdata;

    RegisteredField();
    RegisteredField(const std::string& key, const std::string& type, int maxoccurs,
        const RegisteredField* parent);
public:
    const std::string& getKey() const { return key; }
    void* getWriterData() const { return writerdata; }
    void setWriterData(void* d) { writerdata = d; }
    const RegisteredField* getParent() const { return parent; }
    int getMaxOccurs() const { return maxoccurs; }
    const std::string& getType() const { return type; }
};

class STREAMANALYZER_EXPORT FieldRegister {
private:
    std::map<std::string, RegisteredField*> fields;
public:
    FieldRegister();
    ~FieldRegister();
    const RegisteredField* registerField(const std::string& fieldname,
        const std::string& type, int maxoccurs, const RegisteredField* parent);
    const std::map<std::string, RegisteredField*>& getFields() const {
        return fields;
    }
    std::map<std::string, RegisteredField*>& getFields() {
        return fields;
    }

    static const std::string floatType;
    static const std::string integerType;
    static const std::string binaryType;
    static const std::string stringType;

    static const std::string pathFieldName;
    static const std::string encodingFieldName;
    static const std::string mimetypeFieldName;
    static const std::string filenameFieldName;
    static const std::string extensionFieldName;
    static const std::string embeddepthFieldName;
    static const std::string mtimeFieldName;
    static const std::string sizeFieldName;

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
