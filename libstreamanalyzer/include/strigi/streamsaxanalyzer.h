/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias G. Pfeiffer <tgpfeiffer@web.de>
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
#ifndef STRIGI_STREAMSAXANALYZER_H
#define STRIGI_STREAMSAXANALYZER_H

#include "streamanalyzerfactory.h"

namespace Strigi {
class AnalysisResult;

/**
 * This class is well suited for analyzing XML based file formats, for example
 * SVG files.
 */
class STREAMANALYZER_EXPORT StreamSaxAnalyzer {
    class Private;
    Private* const p;
public:
    /**
     * Constructor.
     */
    StreamSaxAnalyzer();
    /**
     * Destructor. Clean up your room, if dirty :-)
     */
    virtual ~StreamSaxAnalyzer();
    /**
     * Returns the name of this analyzer. Taking the class name is fine
     * for this purpose.
     */
    virtual const char* name() const = 0;
    /**
     * Is called to signal the beginning of a stream analysis. This is the place
     * to initialize variables that need to be set again for every stream.
     * If there are single objects that will be needed again for every analyzed
     * stream, they should be created in the constructor.
     * \param result pointer to the AnalysisResult to write your results to
     */
    virtual void startAnalysis(AnalysisResult*) = 0;
    /**
     * Is called when the analysis of a stream is finished. You can do
     * cleanups here, if necessary. Note: This is also called if, while
     * parsing the XML document, non-well-formedness is detected.
     */
    virtual void endAnalysis(bool complete) = 0;
    /**
     * This is called when an opening XML tag was detected. For documentation
     * purposes, let's suppose we have
     * &lt;xsl:template match="author:*" xmlns:xdc="http://www.xml.com/books"&gt;
     * The parameter descriptions will have in parentheses the value that would be
     * passed for this example.
     * \param localname pointer to local name of the element ("template")
     * \param prefix pointer to element namespace prefix, if available ("xsl"),
     *  to 0 otherwise
     * \param uri pointer to element namespace URI, if available, i.e. if it was
     *  declared in the document element (could be
     *  "http://www.w3.org/1999/XSL/Transform"), to 0 otherwise
     * \param nb_namespaces number of namespace <i>definitions</i> on that node (1)
     * \param namespaces pointer to the array (of length 2*nb_namespaces) of
     *  prefix/URI pairs of namespace <i>definitions</i> (in this case:
     *  ["xdc", "http://www.xml.com/books"])
     * \param nb_attributes number of attributes on that node (1)
     * \param nb_defaulted number of defaulted attributes (0)
     * \param attributes pointer to the array (of length 5*nb_attributes) of
     *  attributes with the following content:<br />
     *  1. items with index i mod 5 == 0 point to the local attribute name ("author")<br />
     *  2. items with index i mod 5 == 1 point to the namespace prefix of the attribute,
     *   if existing, to 0 otherwise (0)<br />
     *  3. items with index i mod 5 == 2 point to the namespace URI of the attribute,
     *   if existing, to 0 otherwise (0)<br />
     *  4. items with index i mod 5 == 3 point to the beginning of the attribute value
     *   in the XML char array. (i.e. to '*')<br />
     *  5. items with index i mod 5 == 4 point to the character <b>after the end</b> of
     *   the attribute value in the XML char array. (i.e. to '"') So if you want
     *   to get the actual attribute value, start reading at the pointer in
     *   (4.) and stop at this one.
     */
    virtual void startElement(const char* localname, const char* prefix,
        const char* uri, int nb_namespaces, const char** namespaces,
        int nb_attributes,int nb_defaulted, const char** attributes);
    /**
     * This is called when a closing XML tag was detected. For documentation
     * purposes, let's suppose we have
     * &lt;/xsl:template&gt;
     * The parameter descriptions will have in parentheses the value that would be
     * passed for this example.
     * \param localname pointer to local name of the element ("template")
     * \param prefix pointer to element namespace prefix, if existing ("xsl"),
     *  to 0 otherwise
     * \param uri pointer to element namespace name if available, i.e. if it
     *  was declared in the document element (could be
     *  "http://www.w3.org/1999/XSL/Transform"), to 0 otherwise
     */
    virtual void endElement(const char* localname, const char* prefix,
        const char* uri);
    /**
     * Is called to pass some XML data to the analyzer. No assumptions like
     * "all data until the next opening or closing tag" etc. can be made!
     * \param data character data of the line
     * \param length number of characters in that line
     */
    virtual void characters(const char* data, uint32_t length);
    /**
     * Tells the caller whether you are finished with your analysis or not.
     * If this returns true, this Analyzer will receive no more data from
     * the stream.
     * If this analyzer notices it is not able to use the given data (for
     * example, this is a completely different file format), have this
     * function return true ASAP.
     * \return true if you are finished with this stream, false otherwise
     */
    virtual bool isReadyWithStream() = 0;
};

/**
 * This is the factory for the creation of a StreamSaxAnalyzer.
 */
class STREAMANALYZER_EXPORT StreamSaxAnalyzerFactory
        : public StreamAnalyzerFactory {
public:
    /**
     * Is called to create a new instance of the corresponding StreamSaxAnalyzer.
     * \return pointer to the new analyzer instance
     */
    virtual StreamSaxAnalyzer* newInstance() const = 0;
};


}

#endif
