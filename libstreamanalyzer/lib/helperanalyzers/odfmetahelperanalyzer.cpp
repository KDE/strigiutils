/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Arend van Beelen jr. <arend@auton.nl>
 * 		 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#include "odfmetahelperanalyzer.h"
#include <strigi/analysisresult.h>
#include <cassert>
#include <cstring>
#include <string>
#include <iostream>

#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include "../rdfnamespaces.h"

using namespace Strigi;
using namespace std;

static const char *dcNS = "http://purl.org/dc/elements/1.1/";
static const char *metaNS = "urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
static const char *opfNS = "http://www.idpf.org/2007/opf";
//static const char *officeNS = "urn:oasis:names:tc:opendocument:xmlns:office:1.0";
//static const char *svgNS = "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0";
//static const char *textNS = "urn:oasis:names:tc:opendocument:xmlns:text:1.0";

const string
    typePropertyName(
	RDF "type"),
    fullnamePropertyName(
	NCO "fullname"),
    creationTimePropertyName(
	NIE "contentCreated"),
    creatorPropertyName(
	NCO "creator"),
    titlePropertyName(
	NIE "title"),
    subjectPropertyName(
	NIE "subject"),
    descriptionPropertyName(
	NIE "description"),
    languagePropertyName(
	NIE "language"),
    keywordPropertyName(
	NIE "keyword"),
    generatorPropertyName(
	NIE "generator"),

    wordCountPropertyName(
	NFO "wordCount"),
    pageCountPropertyName(
	NFO "pageCount"),
    characterCountPropertyName(
	NFO "characterCount"),

    contactClassName(
	NCO "Contact");



/*OdfMetaHelperAnalyzer::registerFields(FieldRegister &reg) {
    tablecountField = reg.registerField("http://strigi.sf.net/ontologies/homeless#documentTableCount");
    paragcountField = reg.registerField("http://freedesktop.org/standards/xesam/1.0/core#paragraphCount");
    objectcountField = reg.registerField("http://strigi.sf.net/ontologies/homeless#documentObjectCount");
    imagecountField = reg.registerField("http://strigi.sf.net/ontologies/homeless#documentImageCount");
}*/

void OdfMetaHelperAnalyzer::startAnalysis(AnalysisResult &r) {
    result = &r;
    m_currentField = NULL;
}

void OdfMetaHelperAnalyzer::endAnalysis(bool /*complete*/) {
    result = NULL;
    m_currentField = NULL;
}

void OdfMetaHelperAnalyzer::startElement(const char *localname, const char *prefix,
                                  const char *uri, int nb_namespaces, const char **namespaces,
                                  int nb_attributes, int nb_defaulted, const char **attributes) {
    assert(result != 0);

    if(uri && strcmp(uri, dcNS) == 0) {
        if(strcmp(localname, "creator") == 0) {
            m_currentField = &creatorPropertyName;
        } else if(strcmp(localname, "title") == 0) {
            m_currentField = &titlePropertyName;
        } else if(strcmp(localname, "subject") == 0) {
            m_currentField = &subjectPropertyName;
        } else if(strcmp(localname, "description") == 0) {
            m_currentField = &descriptionPropertyName;
        } else if(strcmp(localname, "language") == 0) {
            m_currentField = &languagePropertyName;
// This element never appears in ODF files. It is defined in OPF specification 
// (see http://www.openebook.org/2007/opf/OPF_2.0_final_spec.html#Section2.2.7 ) used
// in ePub electronic book format. It is here only because metadata format is almost
// identical to one used in ODF, so it makes more sense to reuse ODF analyzer for ePub too than 
// to duplicate whole thing just to add support for one additional field
        } else if((strcmp(localname, "date") == 0) && nb_attributes == 1 &&
          (strcmp(attributes[0], "event") == 0) && attributes[2] != 0 &&
          (strcmp(attributes[2], opfNS) == 0) &&
          (strncmp(attributes[3], "creation", attributes[4] - attributes[3]) == 0 ) ) {
            m_currentField = &creationTimePropertyName;
        }
    } else if(uri && strcmp(uri, metaNS) == 0) {
        if(strcmp(localname, "creation-date") == 0) {
            m_currentField = &creationTimePropertyName;
        } else if(strcmp(localname, "keyword") == 0) {
            m_currentField = &keywordPropertyName;
        } else if(strcmp(localname, "generator") == 0) {
	    m_currentField = &generatorPropertyName;
	} else if(strcmp(localname, "document-statistic")==0) {
	   for(int i = 0 ; i < nb_attributes ;i++)
	   	{
		   if(strcmp(attributes[2+i*5], metaNS) ==0) {
			const char *attrName(attributes[0+i*5]);
			int stringLength = strlen(attributes[3+i*5]) - strlen(attributes[4+i*5]);
			string line(attributes[3+i*5],stringLength);

		   	if(strcmp(attrName, "word-count") ==0 ){
				result->addTriplet(result->path(), wordCountPropertyName, line);
			}
			else if(strcmp(attrName, "paragraph-count") ==0 ){
				//FIXME: either get rid of this or replace with NIE equivalent
				//result->addValue(m_factory->paragcountField,line);
			}
                        else if(strcmp(attrName, "page-count") ==0 ){
				result->addTriplet(result->path(), pageCountPropertyName,line);
                        }
                        else if(strcmp(attrName, "image-count") ==0 ){
				//FIXME: either get rid of this or replace with NIE equivalent
				//result->addValue(m_factory->imagecountField,line);
                        }
                        else if(strcmp(attrName, "character-count") ==0 ){
				result->addTriplet(result->path(), characterCountPropertyName,line);
                        }
                        else if(strcmp(attrName, "object-count") ==0 ){
				//FIXME: either get rid of this or replace with NIE equivalent
				//result->addValue(m_factory->objectcountField,line);
                        }
                        else if(strcmp(attrName, "table-count") ==0 ){
				//FIXME: either get rid of this or replace with NIE equivalent
				//result->addValue(m_factory->tablecountField,line);
                        }
		   }
	   	}
	   }
	}
}

void OdfMetaHelperAnalyzer::endElement(const char *localname, const char *prefix,
                                const char *uri) {
    m_currentField = NULL;
}

void OdfMetaHelperAnalyzer::characters(const char *data, uint32_t length) {
    assert(result != 0);

    if(m_currentField != NULL) {
	if(m_currentField == &creatorPropertyName) {
	    string creatorUri = result->newAnonymousUri();
	    result->addTriplet(result->path(), *m_currentField, creatorUri);
	    result->addTriplet(creatorUri, typePropertyName, contactClassName);
	    result->addTriplet(creatorUri, fullnamePropertyName, string(data,length));
	} else
	    result->addTriplet(result->path(), *m_currentField, string(data, length));
    }
}
