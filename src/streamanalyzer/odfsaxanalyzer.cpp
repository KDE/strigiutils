/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Arend van Beelen jr. <arend@auton.nl>
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

#include "odfsaxanalyzer.h"
#include <cassert>
#include <cstring>
#include <string>

#include "analysisresult.h"
#include "fieldtypes.h"

using namespace Strigi;

static const char *dcNS = "http://purl.org/dc/elements/1.1/";
static const char *metaNS = "urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
static const char *opfNS = "http://www.idpf.org/2007/opf";
//static const char *officeNS = "urn:oasis:names:tc:opendocument:xmlns:office:1.0";
//static const char *svgNS = "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0";
//static const char *textNS = "urn:oasis:names:tc:opendocument:xmlns:text:1.0";

void OdfSaxAnalyzerFactory::registerFields(FieldRegister &reg) {
    creatorField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#author");
    creationTimeField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#contentCreated");
    titleField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#title");
    subjectField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#subject");
    descriptionField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#description");
    languageField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#language");
    keywordField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#contentKeyword");
    generatorField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#generator");
    tablecountField = reg.registerField("document.stats.table_count");
    pagecountField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#pageCount");
    paragcountField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#paragraphCount");
    wordcountField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#wordCount");
    charcountField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#characterCount");
    objectcountField = reg.registerField("document.stats.object_count");
    imagecountField = reg.registerField("document.stats.image_count");
}

Strigi::StreamSaxAnalyzer *OdfSaxAnalyzerFactory::newInstance() const {
    return new OdfSaxAnalyzer(this);
}

OdfSaxAnalyzer::OdfSaxAnalyzer(const OdfSaxAnalyzerFactory *factory) :
  m_factory(factory),
  m_result(0),
  m_currentField(0) {
    assert(m_factory != 0);
}

void OdfSaxAnalyzer::startAnalysis(AnalysisResult *result) {
    assert(result != 0);

    if(result->parent() != 0) {
        m_result = result->parent();
    } else {
        m_result = result;
    }
}

void OdfSaxAnalyzer::endAnalysis(bool /*complete*/) {
    m_result = 0;
}

void OdfSaxAnalyzer::startElement(const char *localname, const char *prefix,
                                  const char *uri, int nb_namespaces, const char **namespaces,
                                  int nb_attributes, int nb_defaulted, const char **attributes) {
    assert(m_result != 0);

    if(uri && std::strcmp(uri, dcNS) == 0) {
        if(std::strcmp(localname, "creator") == 0) {
            m_currentField = m_factory->creatorField;
        } else if(std::strcmp(localname, "title") == 0) {
            m_currentField = m_factory->titleField;
        } else if(std::strcmp(localname, "subject") == 0) {
            m_currentField = m_factory->subjectField;
        } else if(std::strcmp(localname, "description") == 0) {
            m_currentField = m_factory->descriptionField;
        } else if(std::strcmp(localname, "language") == 0) {
            m_currentField = m_factory->languageField;
// This element never appears in ODF files. It is defined in OPF specification 
// (see http://www.openebook.org/2007/opf/OPF_2.0_final_spec.html#Section2.2.7 ) used
// in ePub electronic book format. It is here only because metadata format is almost
// identical to one used in ODF, so it makes more sense to reuse ODF analyzer for ePub too than 
// to duplicate whole thing just to add support for one additional field
        } else if((std::strcmp(localname, "date") == 0) && nb_attributes == 1 && 
          (std::strcmp(attributes[0], "event") == 0) && attributes[2] != 0 &&
          (std::strcmp(attributes[2], opfNS) == 0) &&
          (std::strncmp(attributes[3], "creation", attributes[4] - attributes[3]) == 0 ) ) { 
            m_currentField = m_factory->creationTimeField; 
        }
    } else if(uri && std::strcmp(uri, metaNS) == 0) {
        if(std::strcmp(localname, "creation-date") == 0) {
            m_currentField = m_factory->creationTimeField;
        } else if(std::strcmp(localname, "keyword") == 0) {
            m_currentField = m_factory->keywordField;
        } else if(std::strcmp(localname, "generator") == 0) {
	    m_currentField = m_factory->generatorField;
	} else if(std::strcmp(localname, "document-statistic")==0) {
	   for(int i = 0 ; i < nb_attributes ;i++)
	   	{
		   if(std::strcmp(attributes[2+i*5], metaNS) ==0) {
			const char *attrName(attributes[0+i*5]);
			int stringLength = std::strlen(attributes[3+i*5]) - std::strlen(attributes[4+i*5]);
			std::string line(attributes[3+i*5],stringLength);

		   	if(std::strcmp(attrName, "word-count") ==0 ){
				m_result->addValue(m_factory->wordcountField, line);
			}
			else if(std::strcmp(attrName, "paragraph-count") ==0 ){
				m_result->addValue(m_factory->paragcountField,line);
			}
                        else if(std::strcmp(attrName, "page-count") ==0 ){
				m_result->addValue(m_factory->pagecountField,line);
                        }
                        else if(std::strcmp(attrName, "image-count") ==0 ){
				m_result->addValue(m_factory->imagecountField,line);
                        }
                        else if(std::strcmp(attrName, "character-count") ==0 ){
				m_result->addValue(m_factory->charcountField,line);
                        }
                        else if(std::strcmp(attrName, "object-count") ==0 ){
				m_result->addValue(m_factory->objectcountField,line);
                        }
                        else if(std::strcmp(attrName, "table-count") ==0 ){
				m_result->addValue(m_factory->tablecountField,line);
                        }
		   }
	   	}
	   }
	}
}

void OdfSaxAnalyzer::endElement(const char *localname, const char *prefix,
                                const char *uri) {
    m_currentField = 0;
}

void OdfSaxAnalyzer::characters(const char *data, uint32_t length) {
    assert(m_result != 0);

    if(m_currentField != 0) {
        m_result->addValue(m_currentField, data, length);
    }
}
