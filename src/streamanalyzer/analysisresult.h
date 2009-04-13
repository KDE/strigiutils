/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006,2009 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_ANALYSISRESULT_H
#define STRIGI_ANALYSISRESULT_H

#include <string>

#include <strigi/strigiconfig.h>
#include "streambase.h"

#if defined(__GNUC__) && __GNUC__ >= 3 && __GNUC_MINOR__ >= 2
#define STRIGI_DEPRECATED __attribute__((deprecated))
#else
#define STRIGI_DEPRECATED
#endif

/**
 * Strigi is the major namespace for all classes that are used in the analysis of streams.
 */
namespace Strigi {

class IndexWriter;
class AnalyzerConfiguration;
class StreamAnalyzer;
class RegisteredField;
class StreamEndAnalyzer;

/**
 * Indexed representation of a file.
 *
 * This object allows StreamEndAnalyzer and StreamThroughAnalyer instances to
 * write data associated with a document to the index. The data is
 * automatically written do the index when ~AnalysisResult() is called.
 **/
class STREAMANALYZER_EXPORT AnalysisResult {
friend class StreamAnalyzerPrivate;
private:
    class Private;
    Private* const p;

    /**
     * @brief Create a new AnalysisResult object that will be written to the index.
     *
     * @param path the path of the file
     * @param name the name of the file
     * @param mt the last modified time of the file
     * @param d the depth at which a document is embedded in other documents.
     *        a depth of 0 means a document is not embedded in another document.
     **/
    AnalysisResult(const std::string& path, const char* name, time_t mt,
        AnalysisResult& parent);
    /**
     * @brief Retrieve the type of end analyzer an analysisresult has.
     *
     * This is useful for determining the filetype of the parent.
     */
    void setEndAnalyzer(const StreamEndAnalyzer*);
public:
    /**
     * @brief Create a new AnalysisResult object that will be written to the index.
     *
     * @param path the path of the file
     * @param mt the last modified time of the file
     * @param writer the writer with which the analysis result will be written upon
     *        destruction
     **/
    AnalysisResult(const std::string& p, time_t mt, IndexWriter& w,
            StreamAnalyzer& analyzer, const std::string& parent = "");
    /**
     * @brief Write the analysis result to the index and release the allocated resources.
     **/
    ~AnalysisResult();
    /**
     * @brief Parse the given stream and index the results into
     * this AnalysisResult object.
     *
     * @param file the stream providing the contents of the file
     * corresponding to this AnalysisResult.
     *
     * @return 0 on success, a negative value on error
     **/
    signed char index(StreamBase<char>* file);
    /**
     * @brief Parse the given stream, treating it as a child of the
     * stream corresponding to this object.
     *
     * For example, if this AnalysisResult corresponds to a tar
     * archive, this function would be called to index a file
     * contained in that archive.
     *
     * @param name the name of the file corresponding to @p file
     * @param mt the last modified time of the file
     * @param file the InputStream for this file
     *
     * @return 0 on success, a negative value on error
     **/
    signed char indexChild(const std::string& name, time_t mt,
        StreamBase<char>* file);
    /**
     * @brief return a pointer to the last child that was indexed
     * Calling indexChild() creates a child AnalysisResult. The pointer returned
     * by this function is valid until the next call to indexChid() or until
     * this instance is destroyed.
     * @return a pointer to the last child that was indexed or NULL if no
     *         child was indexed yet
     * @since Strigi 0.6.5
     **/
    AnalysisResult* child();
    /**
     * Associate a fragment of text with the file.
     *
     * @param text    a pointer to a fragment of utf8 encoded text
     * @param length  the length of the fragment
     **/
    void addText(const char* text, int32_t length);
    /**
     * Associate a value for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * @param field  the registered field
     * @param value  utf8 string value
     **/
    void addValue(const RegisteredField* field, const std::string& value);
    /**
     * Associate a value for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * @param field   the registered field
     * @param value   byte array
     * @param length  the length of the array
     **/
    void addValue(const RegisteredField* field, const char* data,
        uint32_t length);
    /**
     * Associate a value for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * @param field  the registered field
     * @param value  the value to add
     **/
    void addValue(const RegisteredField* field, uint32_t value);
    /**
     * Associate a value for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * @param field  the registered field
     * @param value  the value to add
     **/
    void addValue(const RegisteredField* field, int32_t value);
    /**
     * Associate a value for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * @param field  the registered field
     * @param value  the value to add
     **/
    void addValue(const RegisteredField* field, double value);
    /**
     * Associate an RDF triplet with the file.
     *
     * @param subject
     * @param predicate
     * @param object
     **/
    void addTriplet(const std::string& subject, const std::string& predicate,
        const std::string& object);
    /**
     * Associate a name/value pair for a field with the file.
     *
     * addValue() may only be called <tt>field->properties.maxCardinality()</tt> times
     * for any given field on any given AnalysisResult.
     *
     * WARNING: this is currently not implemented.
     *
     * @param field  the registered field
     * @param name   the name to add
     * @param value  the value to add
     **/
    void addValue(RegisteredField*field, const std::string& name,
        const std::string& value);
    /**
     * @brief Generate a new "anonymous" uri.
     *
     * Anonymous uri is later supposed to be replaced by inferencer with a canonical uri.
     **/
    std::string newAnonymousUri();
    /**
     * @brief Get the filename of the associated file.
     *
     * For the full name see path().
     **/
    const std::string& fileName() const;
    /**
     * @brief Get the path of the associated file.
     **/
    const std::string& path() const;
    /**
     * @brief Get the path of the file associated with the parent.
     **/
    const std::string& parentPath() const;
    /**
     * @brief Get the last modified time of the associated file.
     */
    time_t mTime() const;
    /**
     * @brief Get the depth of the associated files in other files.
     *
     * When a file that may contain other files (such as a tar archive)
     * is indexed, the files contained in it are also indexed.  Each
     * file has its own AnalysisResult associated with it.
     * This function returns the depth of this containment for the file
     * associated with this AnalysisResult.
     *
     * @param the depth of containment of the associated file. Since the depth
     *        should not be too large, the type of depth is restricted to 'char'
     */
    signed char depth() const;
    /**
     * @deprecated
     * @brief Use writerData() instead.
     */
    STRIGI_DEPRECATED int64_t id() const;
    /**
     * @deprecated
     * @brief Use setWriterData() instead.
     */
    STRIGI_DEPRECATED void setId(int64_t i);
    /**
     * @brief Set the encoding of the file associated with
     * this analysis result.
     *
     * @param enc the name of the encoding
     */
    void setEncoding(const char* enc);
    /**
     * @brief Get the encoding of the file associated with
     * this analysis result.
     *
     * @return the name of the encoding
     */
    const std::string& encoding() const;
    /**
     * @brief Get the parent of this analysis result.
     *
     * If the file associated with this analysis was contained
     * within another analyzed file, this returns a pointer to
     * the analysis result associated with the file it is
     * immediately contained in.
     *
     * For example, if this file is a member of a tar archive, this
     * will return the analysis result associated with the tar archive.
     *
     * @return pointer to the analysis result associated with the parent
     * file of this analysis result, or 0 if there is no such analysis
     * result
     */
    AnalysisResult* parent();
    const AnalysisResult* parent() const;
    /**
     * @brief Retrieve the IndexWriter specific object
     * associated with this AnalysisResult.
     *
     * It is up to the caller to know the correct cast to
     * perform on the returned pointer.
     *
     * See AnalysisResult::setWriterData() for more details.
     **/
    void* writerData() const;
    /**
     * @brief Set the IndexWriter specific object associated with this AnalysisResult.
     *
     * This object allows the IndexWriter to store intermediate results
     * associated with this AnalysisResult. IndexWriters using this feature should
     * initialize this value in IndexWriter::startAnalysis() and should
     * deallocate the value in the call to IndexWriter::finishAnalysis().
     * Each of these functions will be called once during the lifetime of each
     * AnalysisResult.
     *
     * @param data pointer to the object
     **/
    void setWriterData(void* data) const;
    /**
     * @brief Set the mimetype of the associated file.
     */
    void setMimeType(const std::string& mt);
    /**
     * @brief Get the mimetype of the associated file.
     */
    const std::string& mimeType() const;
    /**
     * @brief Get the extension of the associated file.
     *
     * May be an empty string.
     */
    std::string extension() const;
    /**
     * @brief Get the configuration object for this
     * analysis result.
     */
    AnalyzerConfiguration& config() const;
    /**
     * @brief Get the end analyzer that is being run on
     * the associated file.
     */
    const StreamEndAnalyzer* endAnalyzer() const;
};

} // end namespace Strigi

#endif
