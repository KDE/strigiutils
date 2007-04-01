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
#include <map>
#include <string>

namespace Strigi {

class RegisteredField {
friend class FieldRegister;
private:
    const std::string m_key;
    const std::string m_type;
    const int m_maxoccurs;
    const RegisteredField* m_parent;
    void* m_writerdata;

    RegisteredField();
    RegisteredField(const std::string& key, const std::string& type,
        int maxoccurs, const RegisteredField* parent);
public:
    const std::string& key() const { return m_key; }
    void* writerData() const { return m_writerdata; }
    void setWriterData(void* data) { m_writerdata = data; }
    const RegisteredField* parent() const { return m_parent; }
    int maxOccurs() const { return m_maxoccurs; }
    const std::string& type() const { return m_type; }
};

class STREAMANALYZER_EXPORT FieldRegister {
private:
    std::map<std::string, RegisteredField*> m_fields;
public:
    FieldRegister();
    ~FieldRegister();
    const RegisteredField* registerField(const std::string& fieldname,
        const std::string& type, int maxoccurs, const RegisteredField* parent);
    const std::map<std::string, RegisteredField*>& fields() const {
        return m_fields;
    }
    std::map<std::string, RegisteredField*>& fields() {
        return m_fields;
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
