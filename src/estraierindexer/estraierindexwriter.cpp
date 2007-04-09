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
#include "estraierindexwriter.h"
#include "estraierindexmanager.h"
#include "analysisresult.h"
#include <fieldtypes.h>
#include <vector>
#include <sstream>
using namespace std;
using namespace Strigi;

struct EstraierData {
    ESTDOC* doc;
    int nwords;
};

EstraierIndexWriter::EstraierIndexWriter(EstraierIndexManager *m)
        : manager(m) {
}
EstraierIndexWriter::~EstraierIndexWriter() {
    // make sure the cache is empty
    commit();
}
void
EstraierIndexWriter::addText(const AnalysisResult* idx, const char* text,
        int32_t length) {
    ESTDOC* doc = static_cast<ESTDOC*>(idx->writerData());;
    string value(text, length);
    est_doc_add_text(doc, value.c_str());
}
void
EstraierIndexWriter::addValue(const AnalysisResult* idx,
        const RegisteredField* field, const string& value) {
    ESTDOC* doc = static_cast<ESTDOC*>(idx->writerData());
    if (field->getKey() == "size") {
        est_doc_add_attr(doc, "@size", value.c_str());
    } else if (field->getKey() == "title") {
        est_doc_add_attr(doc, "@title", value.c_str());
    } else {
        est_doc_add_attr(doc, field->getKey().c_str(), value.c_str());
    }
}
void
EstraierIndexWriter::startAnalysis(AnalysisResult* idx) {
    // allocate a new estraier document
    ESTDOC* doc = est_doc_new();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
EstraierIndexWriter::finishAnalysis(const AnalysisResult* idx) {
    ESTDOC* doc = static_cast<ESTDOC*>(idx->writerData());;
    // add required url field

    est_doc_add_attr(doc, "@uri", idx->path().c_str());
    char numbuf[64];
    sprintf(numbuf, "%llu", (int64_t)idx->mTime());
    est_doc_add_attr(doc, "@mdate", numbuf);
    sprintf(numbuf, "%i", idx->depth());
    est_doc_add_attr(doc, "depth", numbuf);

    ESTDB* db = manager->ref();
    int ok = est_db_put_doc(db, doc, 0);
    if (!ok) {
        fprintf(stderr, "error writing document: %s\n",
            est_err_msg(est_db_error(db)));
    }
    manager->deref();
    // deallocate the estraier document
    est_doc_delete(doc);
}
void
EstraierIndexWriter::commit() {
    ESTDB* db = manager->ref();
    //est_db_optimize(db, 0);
    est_db_sync(db);
    manager->deref();
}
/**
 * Delete all files that start with the specified path.
 **/
void
EstraierIndexWriter::deleteEntries(const std::vector<std::string>& entries) {

    // retrieve the ids of all documents
    int n;
    int* all;
    ESTCOND* c = est_cond_new();
    est_cond_add_attr(c, "@id NUMGE 0");
    ESTDB* db = manager->ref();
    all = est_db_search(db, c, &n, NULL);
    est_cond_delete(c);

    // loop over all documents and check if they should be deleted
    vector<string>::const_iterator j;
    for (int i=0; i<n; ++i) {
        int id = all[i];
        char* uri = est_db_get_doc_attr(db, id, "@uri");
        uint len = strlen(uri);
        for (j = entries.begin(); j != entries.end(); ++j) {
            if (j->length() <= len
                    && strncmp(j->c_str(), uri, j->length()) == 0) {
                est_db_out_doc(db, id, 0);
                break;
            }
        }
        free(uri);
    }
    free(all);
    manager->deref();
}
void
EstraierIndexWriter::deleteAllEntries() {
    manager->deleteIndex();
}
