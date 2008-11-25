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
#ifndef STRIGI_ANALYZERCONFIGURATION_H
#define STRIGI_ANALYZERCONFIGURATION_H

#include "streamendanalyzer.h"
#include "streamsaxanalyzer.h"
#include "streamlineanalyzer.h"
#include "streamthroughanalyzer.h"
#include "streameventanalyzer.h"
#include "fieldtypes.h"

namespace Strigi {
class AnalyzerConfigurationPrivate;
/**
 * @brief This class provides information and functions to control
 * the analysis.
 *
 * For example, it allows the files to be indexed to be limited based
 * on the name and path of the files.  It also stores the field
 * register (see AnalyzerConfiguration::fieldRegister and
 * Strigi::FieldRegister).
 *
 * It can be subclassed to provide finer control over the analysis
 * process.
 **/
class STREAMANALYZER_EXPORT AnalyzerConfiguration {
public:
/**
 * @brief Provides hints about how the IndexWriter should store
 * a field.
 *
 * This is a flag type - the different attributes can be
 * combined.
 *
 * Note that if neither Indexed nor Stored is set for a field,
 * the field may not be stored in the index.  If it is Stored
 * but not Indexed, the field value will not be able to be
 * searched for quickly, but may be returned as part of the
 * results from a search.
 */
enum FieldType {
    None       = 0x0000 /**< No hint. */,
    Binary     = 0x0001 /**< The field should be stored as binary data. */,
    Compressed = 0x0002 /**< If the field is stored, the data
                             should be compressed. */,
    Indexed    = 0x0004 /**< The field should be indexed. */,
    Stored     = 0x0020 /**< The field should be stored. */,
    Tokenized  = 0x0040 /**< If the field contains text, it
                             should be tokenized. */
};
private:
    AnalyzerConfigurationPrivate* const p;
public:
    AnalyzerConfiguration();
    virtual ~AnalyzerConfiguration();
    /**
     * @brief Whether a given file should be indexed.
     *
     * In the default implementation, the path and filename
     * are checked against the filters specified by setFilters().
     * @p path is used if the filter pattern contains a /,
     * and @p filename is checked otherwise.
     *
     * The default implementation only checks against patterns
     * that do not end with @c /
     *
     * @param path the path to the file (eg: "/folder/a.txt")
     * @param filename the name of the file (eg: "a.txt")
     */
    virtual bool indexFile(const char* path, const char* filename) const;
    /**
     * @brief Whether a given directory should be indexed.
     *
     * In the default implementation, the path and filename
     * are checked against the filters specified by setFilters().
     * @p path is used if the filter pattern contains a /,
     * and @p filename is checked otherwise.
     *
     * The default implementation only checks against patterns
     * ending with @c /
     *
     * @param path the path to the directory, including
     * the directory name
     * @param filename the name of the directory
     */
    virtual bool indexDir(const char* path, const char* filename) const;
    /**
     * @brief Whether to use the given factory.
     *
     * Allows you to prevent the analyzers produced by a particular
     * factory from being used.
     *
     * The default implementation allows all factories.
     */
    virtual bool useFactory(StreamAnalyzerFactory*) const {
        return true;
    }
    /**
     * @brief Whether to use the given factory.
     *
     * This is an overloaded function.  See
     * useFactory(StreamEndAnalyzerFactory*)
     * for more information.
     */
    virtual bool useFactory(StreamEndAnalyzerFactory* f) const {
        return useFactory(static_cast<StreamAnalyzerFactory*>(f));
    }
    /**
     * @brief Whether to use the given factory.
     *
     * This is an overloaded function.  See
     * useFactory(StreamEndAnalyzerFactory*)
     * for more information.
     */
    virtual bool useFactory(StreamThroughAnalyzerFactory* f) const {
        return useFactory(static_cast<StreamAnalyzerFactory*>(f));
    }
    /**
     * @brief Whether to use the given factory.
     *
     * This is an overloaded function.  See
     * useFactory(StreamEndAnalyzerFactory*)
     * for more information.
     */
    virtual bool useFactory(StreamSaxAnalyzerFactory* f) const {
        return useFactory(static_cast<StreamAnalyzerFactory*>(f));
    }
    /**
     * @brief Whether to use the given factory.
     *
     * This is an overloaded function.  See
     * useFactory(StreamEndAnalyzerFactory*)
     * for more information.
     */
    virtual bool useFactory(StreamEventAnalyzerFactory* f) const {
        return useFactory(static_cast<StreamAnalyzerFactory*>(f));
    }
    /**
     * @brief Whether to use the given factory.
     *
     * This is an overloaded function.  See
     * useFactory(StreamEndAnalyzerFactory*)
     * for more information.
     */
    virtual bool useFactory(StreamLineAnalyzerFactory* f) const {
        return useFactory(static_cast<StreamAnalyzerFactory*>(f));
    }
    /**
     * @brief Allows end analyzer to check whether they should continue
     * indexing.
     *
     * This should be called by end analyzers at convenient points to check
     * whether they should continue indexing.  For example, an end analyzer
     * analyzing a tar archive might call this to check whether it should
     * index the archive's children.
     *
     * This can be used to stop the indexing process at the next convenient
     * time.  For example, if the user wishes to interrupt the indexing
     * process, or if the tool @c deepgrep was asked to find the first
     * occurrence of a term and then stop.
     *
     * @return true if indexing should continue, false if it should stop
     */
    virtual bool indexMore() const {return true;}
    bool indexArchiveContents() const;
    /**
     * @brief Allows end analyzer to check whether they should continue
     * adding text fragments to the index.
     *
     * This should be called by end analyzers before adding text
     * fragments with AnalysisResult::addText().
     *
     * This can be used to prevent the text index from being created,
     * or to prevent it from expanding.
     *
     * @return true if more text should be added to the index, false
     * if no more text should be added
     */
    virtual bool addMoreText() const {
        return true;
    }
    /**
     * @brief Return the maximal number of bytes that may be read from the
     * stream whose results are being written into @p ar.
     *
     * This function allows one to do analyses that only look at the first
     * bytes of streams for performance reasons. A scenario could be for getting
     * metadata for showing in a file manager.
     *
     * The individual analyzers should honour the value that is returned from
     * this function. They should also not assume that this value is constant
     * during the analysis and should regularly check whether they have not
     * read too much.
     *
     * @return the maximal number of bytes that may be read, or -1 if there is
     *         no limit
     **/
    virtual int64_t maximalStreamReadLength(const Strigi::AnalysisResult&/*ar*/) {
        return -1;
    }
    /**
     * @brief Determine the field indexing properties of a field.
     *
     * See AnalyzerConfiguration::FieldType for more information.
     *
     * @param f the field to determine the indexing properties for
     */
    virtual FieldType indexType(const Strigi::RegisteredField* f) const;
    /**
     * @brief Set the list of patterns used to filter out files and directories
     * when indexing.
     *
     * These are used in the default implementations of indexFile() and
     * indexDir().  They are parsed in strict order of occurrence in the
     * vector.
     *
     * Each filter is a pair, linking a boolean to a string.  The string
     * should be a shell wildcard pattern (note that wildcards can match
     * @c /, but will not match a leading @c . in a filename).  The boolean
     * indicates whether files that match should be indexed (true) or not
     * indexed (false).
     *
     * If the pattern ends with a @c /, it will only match directories.
     * Otherwise, the pattern will only match files.
     *
     * The first pattern that matches will be used to determine whether
     * a file or directory should be indexed, and subsequent patterns
     * will be ignored.
     *
     * TODO: write proper documentation of the pattern syntax.
     *
     * @param filters a list of pairs of patterns together with whether
     * files or directories matching the pattern should be indexed
     */
    void setFilters(const std::vector<std::pair<bool,std::string> >& filters);
    /**
     * @brief set the list of patterns used to filter out files and directories
     * when indexing.
     *
     * See setFilters() for more details.
     */
    const std::vector<std::pair<bool,std::string> >& filters() const;
    /**
     * @brief Get the field register.
     *
     * This gets the field register for this analysis.  This will contain
     * all the fields that may be assigned to during this analysis.
     * There is no guarantee that all these fields will be assigned to.
     * In fact, it is pretty certain that not all the fields in the
     * field register will be assigned to, since this will depend on the
     * files being analyzed.
     *
     * The field register is populated by the registerFields() function
     * of the analyzer factories.
     *
     * @return the field register
     */
    FieldRegister& fieldRegister();
    /**
     * @brief Get the field register.
     *
     * See the documentation for the non-const version of this function.
     */
    const FieldRegister& fieldRegister() const;

protected:
    /**
     * indexArchiveContents is not virtual to keep binary comp.
     */
    void setIndexArchiveContents( bool );
};

/*
 * Overloaded operator| that retains the type of the flag when |'ing two
 * field values.
 **/
//AnalyzerConfiguration::FieldType
//operator|(AnalyzerConfiguration::FieldType a, AnalyzerConfiguration::FieldType b);

}
#endif
