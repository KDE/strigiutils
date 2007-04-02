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

/**
 * @brief Represents a field in the ontology of a file.
 *
 * This provides an efficient way of referring to a particular
 * item of data to be extracted from a file.  For example,
 * there might be a RegisteredField to store the color depth
 * of an image.
 *
 * Note that internationalization of field name and enumerated
 * values are not done by analyzers, but are dealt with in a
 * seperate file describing the ontology.
 */
class RegisteredField {
friend class FieldRegister;
private:
    const std::string m_key;
    const std::string m_type;
    const int m_maxoccurs;
    const RegisteredField* m_parent;
    void* m_writerdata;

    RegisteredField();
    /**
     * @brief Create a RegisteredField.
     *
     * @param key a (unique) name for what this field represents
     * @param type the type of field (see Strigi::FieldRegister)
     * @param maxoccurs the maximum number of types a field will occur in any
     * given resource
     * @param parent the natural parent field for this field
     */
    RegisteredField(const std::string& key, const std::string& type,
        int maxoccurs, const RegisteredField* parent);
public:
    /**
     * @internal
     * @brief Get the key for this field.
     *
     * The key is a (unique) string indicating what this field
     * represents.
     */
    const std::string& key() const { return m_key; }
    /**
     * @internal
     * @brief Get the data stored in the field.
     *
     * It is up the the caller to perform the correct
     * cast on the result based on the value of type().
     */
    void* writerData() const { return m_writerdata; }
    /**
     * @internal
     * @brief Store data in the field.
     *
     * This will overwrite any previous data.
     *
     * It is up the the caller to store the correct type
     * of data as indicated by type().
     */
    void setWriterData(void* data) { m_writerdata = data; }
    /**
     * @internal
     * @brief Get the parent field of this field.
     */
    const RegisteredField* parent() const { return m_parent; }
    /**
     * @internal
     * @brief Get the maximum number of types a field will occur in any
     * given resource.
     */
    int maxOccurs() const { return m_maxoccurs; }
    /**
     * @internal
     * @brief The type of the data in this field.
     */
    const std::string& type() const { return m_type; }
};

/**
 * @brief A register for storing the fields associated with an
 * analysis.
 *
 * All fields that will be used by an analyzer should be registered
 * with a provided FieldRegister.
 */
class STREAMANALYZER_EXPORT FieldRegister {
private:
    std::map<std::string, RegisteredField*> m_fields;
public:
    FieldRegister();
    ~FieldRegister();
    /**
     * @brief Register a field with the FieldRegister.
     *
     * The returned pointer can be used for calls to
     * Strigi::AnalysisResult::addValue().
     *
     * Note that no i18n should be applied to either the field
     * name nor the field value.  i18n is the responsibility of
     * the client, based on the ontology of the field.
     *
     * @param fieldname a unique name for this field
     * @param type the type of data to be stored in the field.  This should
     * be chosen from the type names that are static members of this class
     * @param maxoccurs the maximum number of types a field will occur in any
     * given resource
     * @param parent the natural parent of the field/
     * @return a pointer to a RegisteredField representing this field
     */
    const RegisteredField* registerField(const std::string& fieldname,
        const std::string& type, int maxoccurs, const RegisteredField* parent);
    /**
     * @brief Get list of fields that have been registered with this object
     * as a map from field names to registered fields.
     */
    const std::map<std::string, RegisteredField*>& fields() const {
        return m_fields;
    }
    /**
     * @brief Get list of fields that have been registered with this object
     * as a map from field names to registered fields.
     */
    std::map<std::string, RegisteredField*>& fields() {
        return m_fields;
    }

    /** The type name to use with registerField for fields that will
     * store a float value */
    static const std::string floatType;
    /** The type name to use with registerField for fields that will
     * store an integer value */
    static const std::string integerType;
    /** The type name to use with registerField for fields that will
     * store binary data */
    static const std::string binaryType;
    /** The type name to use with registerField for fields that will
     * store a string value */
    static const std::string stringType;

    /** The name of a field for storing the path of a file */
    static const std::string pathFieldName;
    /** The name of a field for storing the encoding of a file */
    static const std::string encodingFieldName;
    /** The name of a field for storing the mimetype of a file */
    static const std::string mimetypeFieldName;
    /** The name of a field for storing the name of a file */
    static const std::string filenameFieldName;
    /** The name of a field for storing the extension of a file */
    static const std::string extensionFieldName;
    /** The name of a field for storing the depth at which a file
     * is embedded in other files */
    static const std::string embeddepthFieldName;
    /** The name of a field for storing the time a file was last modified */
    static const std::string mtimeFieldName;
    /** The name of a field for storing the size of a file */
    static const std::string sizeFieldName;

    /** A field for storing the path of a file */
    const RegisteredField* pathField;
    /** A field for storing the encoding of a file */
    const RegisteredField* encodingField;
    /** A field for storing the mimetype of a file */
    const RegisteredField* mimetypeField;
    /** A field for storing the name of a file */
    const RegisteredField* filenameField;
    /** A field for storing the extension of a file */
    const RegisteredField* extensionField;
    /** A field for storing the depth at which a file is embedded
     * in other files */
    const RegisteredField* embeddepthField;
    /** A field for storing the time a file was last modified */
    const RegisteredField* mtimeField;
    /** A field for storing the size of a file */
    const RegisteredField* sizeField;
};

}

#endif
