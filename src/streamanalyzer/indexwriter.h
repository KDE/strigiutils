/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include <string>
#include <vector>
#include "jstreamsconfig.h"
#include "streamanalyzer.h"

namespace Strigi {

template <class T> class StreamBase;
class FieldRegister;

class AnalysisResult;
class RegisteredField;

/**
 * Abstract class that provides write access to a Strigi index.
 *
 * Instances of the class should be obtained by calling the function
 * IndexManager::indexWriter() and should not be used from threads other
 * than the thread that called IndexManager::indexWriter().
 *
 * The lifecycle of an IndexWriter should be:
 * <pre>
 * - create indexwriter
 * for all streams {
 * - create an indexable
 * - add the indexwriter to it
 * - add a stream to the indexable (optional)
 * - add fields to indexable (optional)
 * - delete the indexable
 * }
 * - delete the indexwriter
 * </pre>
 *
 * Where the functions have default implementations, they
 * do not do anything.
 */
class STREAMANALYZER_EXPORT IndexWriter {
friend class AnalysisResult;
protected:
    /**
     * @brief Notifies the IndexWriter that a new stream is being analyzed.
     *
     * @param result the AnalysisResult for the stream that is being
     * analyzed.
     */
    virtual void startAnalysis(AnalysisResult*) = 0;
    /**
     * @brief Add a fragment of text to the index.
     *
     * See AnalysisResult::addText() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param text a pointer to a fragment of utf8 encoded text
     * @param length the length of the fragment
     */
    virtual void addText(const AnalysisResult* result, const char* text, int32_t length)=0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param value value of the field
     */
    virtual void addField(const AnalysisResult* result, const RegisteredField* field,
        const std::string& value) = 0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param data value of the field
     * @param size length of the data
     */
    virtual void addField(const AnalysisResult* result, const RegisteredField* field,
        const unsigned char* data, uint32_t size) = 0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param value value of the field
     */
    virtual void addField(const AnalysisResult* result, const RegisteredField* field,
        int32_t value) = 0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param value value of the field
     */
    virtual void addField(const AnalysisResult* result, const RegisteredField* field,
        uint32_t value) = 0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param value value of the field
     */
    virtual void addField(const AnalysisResult* result, const RegisteredField* field,
        double value) = 0;
    /**
     * @brief Add a field to the index.
     *
     * See AnalysisResult::addValue() for more information.
     *
     * @param result the AnalysisResult for the object that is
     * being analyzed
     * @param field description of the field
     * @param name
     * @param value value of the field
     */
    virtual void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& name, const std::string& value) = 0;
    /**
     * @brief Notifies the IndexWriter that the analysis of this
     * stream is complete.
     *
     * @param result the AnalysisResult for the stream that has finished
     * being analyzed.
     */
    virtual void finishAnalysis(const AnalysisResult* result) = 0;
    /**
     * @brief Add a complete RDF triplet.
     *
     * @param subject
     * @param predicate
     * @param object
     **/
    virtual void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) = 0;
public:
    virtual ~IndexWriter() {}
    /**
     * @brief Flush the accumulated changes to disk.
     **/
    virtual void commit() { return; }
    /**
     * @brief Delete the entries with the given paths from the index.
     *
     * @param entries the paths of the files that should be deleted
     **/
    virtual void deleteEntries(const std::vector<std::string>& entries) = 0;
    /**
     * @brief Delete all indexed documents from the index.
     **/
    virtual void deleteAllEntries() = 0;
    /**
     * @brief Return the number of objects that are currently in the cache.
     **/
    virtual int itemsInCache() { return 0; }
    /**
     * @brief Optimize the index.
     *
     * This can be computationally intensive and may cause the index to
     * temporarily use the double amount of diskspace.
     **/
    virtual void optimize() {}
    virtual void initWriterData(const Strigi::FieldRegister& fieldRegister) {}
    virtual void releaseWriterData(const Strigi::FieldRegister& fieldRegister) {}
};

} // end namespace Strigi

#endif
