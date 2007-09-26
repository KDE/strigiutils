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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "cluceneindexwriter.h"
#include <CLucene.h>
#include <CLucene/store/Lock.h>
#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
#include <CLucene/util/stringreader.h>
#include <sstream>
#include <assert.h>

#ifdef STRIGI_USE_CLUCENE_COMPRESSEDFIELDS
#include "jsgzipcompressstream.h"
#endif

#include <iostream>

using lucene::document::Document;
using lucene::document::Field;
using lucene::index::IndexWriter;
using lucene::index::Term;
using lucene::index::TermDocs;
using lucene::search::IndexSearcher;
using lucene::search::Hits;
using lucene::util::BitSet;

using lucene::util::Reader;
using namespace std;
using namespace Strigi;

struct CLuceneDocData {
    lucene::document::Document doc;
    std::string content;
};

CLuceneIndexWriter::CLuceneIndexWriter(CLuceneIndexManager* m):
    manager(m), doccount(0) {
    addMapping(_T(""),_T("content"));
}
CLuceneIndexWriter::~CLuceneIndexWriter() {
}
void
CLuceneIndexWriter::addText(const AnalysisResult* idx, const char* text,
        int32_t length) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    doc->content.append(text, length);
}

#ifdef _UCS2
typedef map<wstring, wstring> CLuceneIndexWriterFieldMapType;
#else
typedef map<string, string> CLuceneIndexWriterFieldMapType;
#endif
CLuceneIndexWriterFieldMapType CLuceneIndexWriterFieldMap;

void CLuceneIndexWriter::addMapping(const TCHAR* from, const TCHAR* to){
    CLuceneIndexWriterFieldMap[from] = to;
}
const TCHAR*
CLuceneIndexWriter::mapId(const TCHAR* id) {
    if (id == 0) id = _T("");
    CLuceneIndexWriterFieldMapType::iterator itr
        = CLuceneIndexWriterFieldMap.find(id);
    if (itr == CLuceneIndexWriterFieldMap.end()) {
        return id;
    } else {
        return itr->second.c_str();
    }
}
void
CLuceneIndexWriter::addValue(const AnalysisResult* idx,
        AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const TCHAR* value) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    Field* field = new Field(name, value,
        (type & AnalyzerConfiguration::Stored) == AnalyzerConfiguration::Stored,
        (type & AnalyzerConfiguration::Indexed) == AnalyzerConfiguration::Indexed,
        (type & AnalyzerConfiguration::Tokenized) == AnalyzerConfiguration::Tokenized);
    doc->doc.add(*field);
}
void
CLuceneIndexWriter::addValue(const AnalysisResult* idx,
        AnalyzerConfiguration::FieldType type, const TCHAR* fn,
        const std::string& value) {
#if defined(_UCS2)
    addValue(idx, type, CLuceneIndexWriter::mapId(fn),
        utf8toucs2(value).c_str());
#else
    addValue(idx, type, CLuceneIndexWriter::mapId(fn), value.c_str());
#endif
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, const std::string& value) {
    AnalyzerConfiguration::FieldType type
        = idx->config().indexType(field);
    if (type == AnalyzerConfiguration::None) return;
#if defined(_UCS2)
    addValue(idx, type, utf8toucs2(field->key()).c_str(), value);
#else
    addValue(idx, type, field->key(), value);
#endif
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, uint32_t value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, int32_t value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field,
        const unsigned char* data, uint32_t size) {
    addValue(idx, field, string((const char*)data, (string::size_type)size));
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, double value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::startAnalysis(const AnalysisResult* idx) {
    doccount++;
    CLuceneDocData*doc = new CLuceneDocData();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishAnalysis(const AnalysisResult* idx) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    wstring c(utf8toucs2(doc->content));
    jstreams::StringReader<char>* sr = NULL; //we use this for compressed streams

    if (doc->content.length() > 0) {
        const TCHAR* mappedFn = mapId(_T(""));
#if defined(_UCS2)
    #ifndef STRIGI_USE_CLUCENE_COMPRESSEDFIELDS
        doc->doc.add(*Field::Text(mappedFn, c.c_str(), false));
    #else
        // lets store the content as utf8. remember, the stream is required
        // until the document is added, so a static construction of stringreader
        // is not good enough
        sr = new jstreams::StringReader<char>(doc->content.c_str(), doc->content.length(), false);

    // add the stored field with the zipstream
    doc->doc.add(*new Field(mappedFn, new jstreams::GZipCompressInputStream(sr),
        Field::STORE_YES));

    // add the tokenized/indexed field
    doc->doc.add(*new Field::Text(mappedFn, c.c_str(),
            Field::STORE_NO | Field::INDEX_TOKENIZED));
    #endif
#else //_UCS2
        doc->doc.add(*Field::Text(mappedFn, doc->content.c_str()) );
#endif
    }
    lucene::index::IndexWriter* writer = manager->refWriter();
    if (writer) {
        try {
            writer->addDocument(&doc->doc);
        } catch (CLuceneError& err) {
            fprintf(stderr, "%s: %s\n", idx->path().c_str(), err.what());
        }
    }
    manager->derefWriter();
    delete doc;
    if ( sr )
        delete sr;
    manager->setIndexMTime();
}
void
CLuceneIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
    manager->closeWriter();
    if (!manager->luceneReader()->checkReader()) {
        fprintf(stderr,"cannot delete entry: lucene reader cannot be opened\n");
        return;
    }
    lucene::index::IndexReader* reader = manager->luceneReader()->reader;
    
    for (uint i=0; i<entries.size(); ++i) {
        deleteEntry(entries[i], reader);
    }
    reader->commit();
    manager->setIndexMTime();
}
void
CLuceneIndexWriter::deleteEntry(const string& entry,
        lucene::index::IndexReader* reader) {

    wstring tstr(utf8toucs2(entry));
    int32_t prefixLen = tstr.length();
    const TCHAR* prefixText = tstr.c_str();
    int32_t maxdoc = reader->maxDoc();
    for (int32_t i = 0; i < maxdoc; ++i) {
        if (!reader->isDeleted(i)) {
            Document* d = reader->document(i);
            const TCHAR* t = d->get(_T("system.location"));
            if (t && _tcsncmp(t, prefixText, prefixLen) == 0) {
                try {
                    reader->deleteDocument(i);
                } catch (...) {
                    fprintf(stderr, "could not delete document");
                }
            }
            _CLDELETE(d);
        }
    }
}
void
CLuceneIndexWriter::deleteAllEntries() {
    manager->deleteIndex();
}
void
CLuceneIndexWriter::commit() {
    manager->closeWriter();
}

//this function is in 0.9.17, which we do not have yet...
bool isLuceneFile(const char* filename){
    if ( !filename )
        return false;
    size_t len = strlen(filename);
    if ( len < 6 ) //need at least x.frx
        return false;
    const char* ext = filename + len;
    while ( *ext != '.' && ext != filename )
        ext--;

    if ( strcmp(ext, ".cfs") == 0 )
        return true;
    else if ( strcmp(ext, ".fnm") == 0 )
        return true;
    else if ( strcmp(ext, ".fdx") == 0 )
        return true;
    else if ( strcmp(ext, ".fdt") == 0 )
        return true;
    else if ( strcmp(ext, ".tii") == 0 )
        return true;
    else if ( strcmp(ext, ".tis") == 0 )
        return true;
    else if ( strcmp(ext, ".frq") == 0 )
        return true;
    else if ( strcmp(ext, ".prx") == 0 )
        return true;
    else if ( strcmp(ext, ".del") == 0 )
        return true;
    else if ( strcmp(ext, ".tvx") == 0 )
        return true;
    else if ( strcmp(ext, ".tvd") == 0 )
        return true;
    else if ( strcmp(ext, ".tvf") == 0 )
        return true;
    else if ( strcmp(ext, ".tvp") == 0 )
        return true;

    else if ( strcmp(filename, "segments") == 0 )
        return true;
    else if ( strcmp(filename, "segments.new") == 0 )
        return true;
    else if ( strcmp(filename, "deletable") == 0 )
        return true;

    else if ( strncmp(ext,".f",2)==0 ){
        const char* n = ext+2;
        if ( *n && _istdigit(*n) )
            return true;
    }

    return false;
}

void
CLuceneIndexWriter::cleanUp() {
    // remove all unused lucene file elements...
    // unused elements are the result of unexpected shutdowns...
    // this can add up to a lot of after a while.

    lucene::index::IndexReader* reader = manager->luceneReader()->reader;
    if (!reader) {
        return;
    }
    lucene::store::Directory* directory = reader->getDirectory();

    // Instantiate SegmentInfos
    lucene::store::LuceneLock* lock = directory->makeLock("commit.lock");
#ifdef LUCENE_COMMIT_LOCK_TIMEOUT
    // version <0.9.16
    bool locked = lock->obtain(LUCENE_COMMIT_LOCK_TIMEOUT);
#else
    bool locked = lock->obtain(lucene::index::IndexWriter::COMMIT_LOCK_TIMEOUT);
#endif
    if (!locked) {
        return;
    }
    lucene::index::SegmentInfos infos;
    try {
        //Have SegmentInfos read the segments file in directory
        infos.read(directory);
    } catch(...) {
        lock->release();
        return; //todo: this may suggest an error...
    }
    lock->release();

    int i;
    set<string> segments;
    for (i = 0; i < infos.size(); i++) {
            lucene::index::SegmentInfo* info = infos.info(i);
            segments.insert(info->name);
    }

    char** files = directory->list();
    char tmp[CL_MAX_PATH];
    for (i = 0; files[i] != NULL; ++i) {
        char* file = files[i];

        int fileLength = strlen(file);
        if ( fileLength < 6 ) {
            continue;
        }

        if (strncmp(file,"segments", 8) == 0
                || strncmp(file, "deletable", 9) == 0) {
            continue;
        }
        if (!isLuceneFile(file)) {
            continue;
        }

        strcpy(tmp, file);
        tmp[fileLength-4] = '\0';

        if (segments.find(tmp) != segments.end()) {
            continue;
        }

        directory->deleteFile(file, false);
    }
    for (i = 0; files[i] != NULL; i++) {
        _CLDELETE_CaARRAY(files[i]);
    }
    _CLDELETE_ARRAY(files);
}

void
CLuceneIndexWriter::initWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        i->second->setWriterData(0);
    }
}
void
CLuceneIndexWriter::releaseWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        delete static_cast<int*>(i->second->writerData());
    }
}
