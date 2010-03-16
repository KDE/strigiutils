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
#include "rpmendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/rpminputstream.h>
#include <strigi/subinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/fieldtypes.h>

using namespace Strigi;

/*
 * TODO: add these fields:
 * "Name", i18n("Name"), QVariant::String);
 * "Version", i18n("Version"), QVariant::String);
 * "Release", i18n("Release"), QVariant::Int);
 * "Summary", i18n("Summary"), QVariant::String);
 * Description );
 * "Group", i18n("Group"), QVariant::String);
 * "Size", i18n("Size"), QVariant::Int);
 * Bytes );
 * "Vendor", i18n("Vendor"), QVariant::String );
 * "Packager", i18n("Packager"), QVariant::String );
 * "Archive Offset", i18n("Archive Offset"), QVariant::Int);
 * "Comment", i18n("Comment"), QVariant::String);
 * MultiLine );
 */

void
RpmEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    typeField = reg.typeField;
}

bool
RpmEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return RpmInputStream::checkHeader(header, headersize);
}
signed char
RpmEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    RpmInputStream rpm(in);
    if (idx.config().indexArchiveContents()) {
        InputStream *s = rpm.nextEntry();
        if (rpm.status()) {
            fprintf(stderr, "error: %s\n", rpm.error());
//        exit(1);
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
            idx.indexChild(rpm.entryInfo().filename, rpm.entryInfo().mtime,
                           s);
            s = rpm.nextEntry();
        }
    }
    if (rpm.status() == Error) {
        m_error = rpm.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    idx.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Software");
    return 0;
}

