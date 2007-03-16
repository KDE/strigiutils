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
#include "fieldtypes.h"
using namespace Strigi;
using namespace std;

RegisteredField::RegisteredField(const cnstr& k, const cnstr& t, int m,
        const RegisteredField* p)
        : key(k), type(t), maxoccurs(m), parent(p), writerdata(0) {
}

STREAMANALYZER_EXPORT const cnstr FieldRegister::floatType = "float";
STREAMANALYZER_EXPORT const cnstr FieldRegister::integerType = "integer";
STREAMANALYZER_EXPORT const cnstr FieldRegister::binaryType = "binary";
STREAMANALYZER_EXPORT const cnstr FieldRegister::stringType = "string";

STREAMANALYZER_EXPORT const cnstr FieldRegister::pathFieldName = "path";
STREAMANALYZER_EXPORT const cnstr FieldRegister::encodingFieldName = "encoding";
STREAMANALYZER_EXPORT const cnstr FieldRegister::mimetypeFieldName = "mimetype";
STREAMANALYZER_EXPORT const cnstr FieldRegister::filenameFieldName = "filename";
STREAMANALYZER_EXPORT const cnstr FieldRegister::extensionFieldName = "ext";
STREAMANALYZER_EXPORT const cnstr FieldRegister::embeddepthFieldName = "depth";
STREAMANALYZER_EXPORT const cnstr FieldRegister::mtimeFieldName = "mtime";
STREAMANALYZER_EXPORT const cnstr FieldRegister::sizeFieldName = "size";

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
