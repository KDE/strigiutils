/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#ifndef STRIGI_FIELDTYPES_H
#define STRIGI_FIELDTYPES_H

#include <strigi/strigiconfig.h>
#include "fieldproperties.h"
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
 * separate file describing the ontology.
 */
class RegisteredField {
friend class FieldRegister;
private:
    const std::string m_key;	/**< TODO (neksa): should stay or go? **/
    const std::string m_type;   /**< TODO (neksa): should go! **/
    const int m_maxoccurs;	/**< TODO (neksa): should go! **/
    const RegisteredField* m_parent; /**< TODO (neksa): should go! **/
    void* m_writerdata;		/**< to be used by a Strigi::IndexWriter to store data*/
    const FieldProperties& m_properties; /**< reference to the object holding all FieldProperties for this field **/

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
    /**
     * @brief Create a RegisteredField, loading all the properties from
     * the fieldproperties ontology database
     *
     * @param fieldname a (unique) name for what this field represents
     *   the same name should be used in .fieldproperties files in [] brackets
     */
    RegisteredField(const std::string& key);
public:
    /**
     * @brief Get the key for this field.
     *
     * The key is a (unique) string indicating what this field
     * represents.
     */
    const std::string& key() const { return m_key; }
    /**
     * @brief Get a pointer to the data stored in the field.
     *
     * Retrieves the data stored by setWriterData().
     *
     * It is the caller's responsibility to know the type
     * of data pointed to.
     *
     * @return a pointer to the data stored by setWriterData(),
     * or 0 if setWriterData() has not been called.
     */
    void* writerData() const { return m_writerdata; }
    /**
     * @brief Store data in the field.
     *
     * This will overwrite any previous data.
     *
     * This is meant to be used by a Strigi::IndexWriter to
     * store the data associated with the field in whatever
     * form is convenient to it.  This may be a string, a
     * primitive type, an array or an object of some
     * description.
     *
     * It is up the the caller to keep track of the type of
     * the data that is pointed to.  The RegisteredField does
     * not take ownership of the data, and it the caller's
     * responsibility to ensure that any data allocated with
     * new is deallocated when it is no longer needed.
     *
     * @param data a pointer to the data
     */
    void setWriterData(void* data) { m_writerdata = data; }
    /**
     * @brief Get the parent field of this field.
     */
    const RegisteredField* parent() const { return m_parent; }
    /**
     * @brief Get the pointer to FieldProperties associated with this field.
     */
    const FieldProperties& properties() const { return m_properties; }
    /**
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
     * @brief (OBSOLETE) Register a field with the FieldRegister.
     *
     * TODO (neksa): this call should go or become obsolete
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
     * @brief Register a field with the FieldRegister.
     *
     * It reads the attributes of the field from the fieldproperties 
     * ontology database.
     *
     * If field is not present in the database a warning would be issued,
     * but the field would be registered with the FieldRegister with @p fieldname
     * key and all other parameters reset to default.
     *
     * The returned pointer can be used for calls to
     * Strigi::AnalysisResult::addValue().
     *
     * Note that no i18n should be applied to either the field
     * name nor the field value.  i18n is the responsibility of
     * the client, based on the ontology of the field.
     *
     * @param fieldname a unique name for this field
     * @return a pointer to a RegisteredField representing this field
     */
    const RegisteredField* registerField(const std::string& fieldname);
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

    static const std::string datetimeType;

    /** The name of a field for storing the path of a file */
    static const std::string pathFieldName;
    /** The name of a field for storing the path of a the parent of file */
    static const std::string parentLocationFieldName;
    /** The name of a field for storing the encoding of a file */
    static const std::string encodingFieldName;
    /** The name of a field for storing the mimetype of a file */
    static const std::string mimetypeFieldName;
    /** The name of a field for storing the name of a file */
    static const std::string filenameFieldName;
    /** The name of a field for describing the text representation of a file */
    static const std::string contentFieldName;
    /** The name of a field for storing the extension of a file */
    static const std::string extensionFieldName;
    /** The name of a field for storing the depth at which a file
     * is embedded in other files */
    static const std::string embeddepthFieldName;
    /** The name of a field for storing the time a file was last modified */
    static const std::string mtimeFieldName;
    /** The name of a field for storing the size of a file */
    static const std::string sizeFieldName;
    /** The name of a field for storing rdf:type of the file/data */
    static const std::string typeFieldName;

    /** Default namespace for fields */
    static const std::string defaultNamespace;

    /** A field for storing the path of a file */
    const RegisteredField* pathField;
    /** A field for storing the path of the parent of a file */
    const RegisteredField* parentLocationField;
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
    /** A field for storing rdf:type of the file/data */
    const RegisteredField* typeField;
    /** A field for storing strigi:debugParseError */
    const RegisteredField* parseErrorField;

};

}

#endif
