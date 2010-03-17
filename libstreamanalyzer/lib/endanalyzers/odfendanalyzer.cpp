/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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
#include "odfendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/zipinputstream.h>
#include <strigi/subinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/fieldtypes.h>
#include "../rdfnamespaces.h"
using namespace Strigi;

/* TODO:
At this moment we discard everything in the archive but content.xml, meta.xml and Pictures/ folder.

This means that not all embedded content will be picked up and the analyzer will fail on
obscure(but technically correct) layouts of documents.

This was tested on Koffice 2.* and OpenOffice 3.*
*/

void
OdfEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
}

bool
OdfEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return !strncmp(header, "PK", 2)
	&& !strncmp(header+30, "mimetypeapplication/vnd.oasis.opendocument.", 43);
}

signed char
OdfEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    ZipInputStream zip(in);
    InputStream *s = zip.nextEntry();
    if (zip.status() != Ok) {
        m_error = zip.error();
        return -1;
    }

    while (s) {
	// check if we're done
	int64_t max = idx.config().maximalStreamReadLength(idx);
	if (max != -1 && in->position() > max) {
	    return 0;
	}
	// check if the analysis has been aborted
	if (!idx.config().indexMore()) {
	    return 0;
	}
	if (zip.entryInfo().filename == "mimetype") {
	    const char *buf;
	    size_t nread;
	    
	    if ((nread = s->read(buf, 47, 47)) < 39)
		return -1;
	    if (strncmp(buf, "application/vnd.oasis.opendocument.", 35))
		return -1;
		
	    const char *rdftype;
	    buf += 35;
	    if( nread >= (35+4) && strncmp(buf, "text", 4) == 0 ) {
		rdftype = NFO "TextDocument";
	    } else if ( nread >= (35+12) && strncmp(buf, "presentation", 12) == 0 ) {
		rdftype = NFO "Presentation";
	    } else if ( nread >= (35+11) && strncmp(buf, "spreadsheet", 11) == 0 ) {
		rdftype = NFO "Spreadsheet";
	    } else rdftype = NFO "Document";

	    idx.addValue(factory->typeField, rdftype);
	    
	} else if (zip.entryInfo().filename == "meta.xml") {
	    metaHelper.analyze(idx, s);
	} else if (zip.entryInfo().filename == "content.xml") {
	    contentHelper.analyze(idx,s);
	} else if (zip.entryInfo().filename.substr(0,9) == "Pictures/") {
	    idx.indexChild(zip.entryInfo().filename, zip.entryInfo().mtime, s);
	}
	s = zip.nextEntry();
    }
    if (zip.status() == Error) {
        m_error = zip.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    return 0;
}

