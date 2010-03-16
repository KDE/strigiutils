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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <strigi/analysisresult.h>
#include <strigi/strigiconfig.h>
#include <strigi/indexwriter.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/streamanalyzer.h>
#include <strigi/streambase.h>
#include <strigi/textutils.h>
#include <strigi/strigi_thread.h>
#include <time.h>
#include <string>
#include <cstdlib>
#include <iconv.h>
#include <cassert>
#include <iostream>
#include <map>

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

using namespace Strigi;
using namespace std;

class Latin1Converter {
    iconv_t const conv;
    char* out;
    size_t outlen;
    STRIGI_MUTEX_DEFINE(mutex);

    int32_t _fromLatin1(char*& out, const char* data, size_t len);
    Latin1Converter() :conv(iconv_open("UTF-8", "ISO-8859-1")), outlen(0) {
        STRIGI_MUTEX_INIT(&mutex);
    }
    ~Latin1Converter() {
        iconv_close(conv);
        free(out);
        STRIGI_MUTEX_DESTROY(&mutex);
    }
    static Latin1Converter& converter() {
        static Latin1Converter l;
        return l;
    }
public:
    static int32_t fromLatin1(char*& out, const char* data, int32_t len) {
        return converter()._fromLatin1(out, data, len);
    }
    static void lock() {
        STRIGI_MUTEX_LOCK(&converter().mutex);
    }
    static void unlock() {
        STRIGI_MUTEX_UNLOCK(&converter().mutex);
    }
};
int32_t
Latin1Converter::_fromLatin1(char*& o, const char* data, size_t len) {
    size_t l = 3*len;
    if (outlen < l) {
        out = (char*)realloc(out, l);
        outlen = l;
    } else {
        l = outlen;
    }
    o = out;
    ICONV_CONST char* inp = (char*)data;
    char* outp = out;
    iconv(conv, &inp, &len, &outp, &l);
    return (len == 0) ?(int32_t)(outlen-l) :0;
}

class AnalysisResult::Private {
public:
    int64_t m_id;
    mutable void* m_writerData;
    const time_t m_mtime;
    std::string m_name;
    const std::string m_path;
    const std::string m_parentpath; // only use this value of m_parent == 0
    std::string m_encoding;
    std::string m_mimetype;
    IndexWriter& m_writer;
    const int m_depth;
    StreamAnalyzer& m_indexer;
    AnalyzerConfiguration& m_analyzerconfig;
    AnalysisResult* const m_this;
    AnalysisResult* const m_parent;
    const StreamEndAnalyzer* m_endanalyzer;
    std::map<const Strigi::RegisteredField*, int> m_occurrences;
    AnalysisResult* m_child;

    Private(const std::string& p, const char* name, time_t mt,
        AnalysisResult& t, AnalysisResult& parent);
    Private(const std::string& p, time_t mt, IndexWriter& w,
        StreamAnalyzer& indexer, const string& parentpath, AnalysisResult& t);
    void write();

    bool checkCardinality(const RegisteredField* field);
};

AnalysisResult::Private::Private(const std::string& p, const char* name,
        time_t mt, AnalysisResult& t, AnalysisResult& parent)
            :m_writerData(0), m_mtime(mt), m_name(name), m_path(p),
             m_writer(parent.p->m_writer), m_depth(parent.depth()+1),
             m_indexer(parent.p->m_indexer),
             m_analyzerconfig(parent.p->m_analyzerconfig),
             m_this(&t), m_parent(&parent),
             m_endanalyzer(0), m_child(0) {
    // make sure that the path starts with the path of the parent
    assert(m_path.size() > m_parent->p->m_path.size()+1);
    assert(m_path.compare(0, m_parent->p->m_path.size(), m_parent->p->m_path)
        == 0);
}
AnalysisResult::AnalysisResult(const std::string& path, const char* name,
        time_t mt, AnalysisResult& parent)
        :p(new Private(path, name, mt, *this, parent)) {
    p->m_writer.startAnalysis(this);
    srand((unsigned int)time(NULL));
}
AnalysisResult::Private::Private(const std::string& p, time_t mt,
        IndexWriter& w, StreamAnalyzer& indexer, const string& parentpath,
        AnalysisResult& t)
            :m_writerData(0), m_mtime(mt), m_path(p), m_parentpath(parentpath),
             m_writer(w), m_depth(0), m_indexer(indexer),
             m_analyzerconfig(indexer.configuration()), m_this(&t),
             m_parent(0), m_endanalyzer(0), m_child(0) {
    size_t pos = m_path.rfind('/'); // TODO: perhaps us '\\' on Windows
    if (pos == std::string::npos) {
        m_name = m_path;
    } else {
        if (pos == m_path.size()-1) {
            // assert that there is no trailing '/' unless it is part of a
            // protocol, which means the parent must be "" and the string must
            // end in a colon followed by up to three slashes
            assert(m_parentpath == "");
            size_t i = m_path.size();
            while (--i > 0 && m_path[i] == '/') {}
            assert(i > 0 && m_path[i] == ':');
        }
        m_name = m_path.substr(pos+1);
    }
    // check that the path start with the path of the parent
    // if the path of the parent is set (!= ""), m_path should be 2 characters
    // longer: 1 for the separator and one for the file name.
    // if m_path == "" and m_parentpath == "" then the unix root dir '/' is
    // meant
    assert((m_path.size() == 0 && m_parentpath.size() == 0)
        || (m_path.size() > (m_parentpath.size()+(m_parentpath.size())?1:0)));
    assert(m_path.compare(0, m_parentpath.size(), m_parentpath) == 0);
}
AnalysisResult::AnalysisResult(const std::string& path, time_t mt,
        IndexWriter& w, StreamAnalyzer& indexer, const string& parentpath)
            :p(new Private(path, mt, w, indexer, parentpath, *this)) {
    p->m_writer.startAnalysis(this);
}
AnalysisResult::~AnalysisResult() {
    // delete child before writing and deleting the parent
    delete p->m_child;
    p->write();
    delete p;
}
void
AnalysisResult::Private::write() {
    const FieldRegister& fr = m_analyzerconfig.fieldRegister();
    m_writer.addValue(m_this, fr.pathField, m_path);
    // get the parent directory and store it without trailing slash
    m_writer.addValue(m_this, fr.parentLocationField,
        (m_parent) ?m_parent->path() :m_parentpath);

    if (m_encoding.length()) {
        m_writer.addValue(m_this, fr.encodingField, m_encoding);
    }
    if (m_mimetype.length()) {
        m_writer.addValue(m_this, fr.mimetypeField, m_mimetype);
    }
    if (m_name.length()) {
        m_writer.addValue(m_this, fr.filenameField, m_name);
    }
    string field = m_this->extension();
    if (field.length()) {
        //m_writer.addValue(m_this, fr.extensionField, field);//FIXME: either get rid of this or replace with NIE equivalent
    }
    //This is superfluous. You can use nie:DataObject type to find out whether you've got a file or embedded data
    //m_writer.addValue(m_this, fr.embeddepthField, (int32_t)m_depth);
    m_writer.addValue(m_this, fr.mtimeField, (uint32_t)m_mtime);

    //FIXME a temporary workaround until we have a file(system) analyzer.
    if(m_depth==0) m_writer.addValue(m_this, fr.typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileDataObject"); 
    
    m_writer.finishAnalysis(m_this);
}
const std::string& AnalysisResult::fileName() const { return p->m_name; }
const std::string& AnalysisResult::path() const { return p->m_path; }
const string& AnalysisResult::parentPath() const {
    return (p->m_parent) ?p->m_parent->path() :p->m_parentpath;
}
time_t AnalysisResult::mTime() const { return p->m_mtime; }
signed char AnalysisResult::depth() const { return (signed char)p->m_depth; }
int64_t AnalysisResult::id() const { return p->m_id; }
void AnalysisResult::setId(int64_t i) { p->m_id = i; }
void AnalysisResult::setEncoding(const char* enc) { p->m_encoding = enc; }
const std::string& AnalysisResult::encoding() const { return p->m_encoding; }
void* AnalysisResult::writerData() const { return p->m_writerData; }
void AnalysisResult::setWriterData(void* wd) const { p->m_writerData = wd; }
void AnalysisResult::setMimeType(const std::string& mt) { p->m_mimetype = mt; }
const std::string& AnalysisResult::mimeType() const { return p->m_mimetype; }
signed char
AnalysisResult::index(InputStream* file) {
    return p->m_indexer.analyze(*this, file);
}
signed char
AnalysisResult::indexChild(const std::string& name, time_t mt,
        InputStream* file) {
    // clean up previous child
    delete p->m_child;
    p->m_child = 0;

    std::string path(p->m_path);
    path.append("/");
    path.append(name);
    const char* n = path.c_str() + path.rfind('/') + 1;
    // check if we should index this file by applying the filename filters
    // make sure that the depth variable does not overflow
    if (depth() < 127 && p->m_analyzerconfig.indexFile(path.c_str(), n)) {
        p->m_child = new AnalysisResult(path, n, mt, *this);
        return p->m_indexer.analyze(*p->m_child, file);
    }
    return 0;
}
AnalysisResult*
AnalysisResult::child() {
    return p->m_child;
}
void
AnalysisResult::addText(const char* text, int32_t length) {
    if (checkUtf8(text, length)) {
        p->m_writer.addText(this, text, length);
    } else {
        Latin1Converter::lock();
        char* d;
        int32_t len = Latin1Converter::fromLatin1(d, text, length);
        if (len && checkUtf8(d, len)) {
            p->m_writer.addText(this, d, len);
        } else {
            fprintf(stderr, "'%.*s' is not a UTF8 or latin1 string\n",
                length, text);
        }
        Latin1Converter::unlock();
    }
}
AnalyzerConfiguration&
AnalysisResult::config() const {
    return p->m_analyzerconfig;
}
AnalysisResult*
AnalysisResult::parent() {
    return p->m_parent;
}
const AnalysisResult*
AnalysisResult::parent() const {
    return p->m_parent;
}
const StreamEndAnalyzer*
AnalysisResult::endAnalyzer() const {
    return p->m_endanalyzer;
}
void
AnalysisResult::setEndAnalyzer(const StreamEndAnalyzer* ea) {
    p->m_endanalyzer = ea;
}
string
AnalysisResult::extension() const {
    string::size_type p1 = p->m_name.rfind('.');
    string::size_type p2 = p->m_name.rfind('/');
    if (p1 != string::npos && (p2 == string::npos || p1 > p2)) {
        return p->m_name.substr(p1+1);
    }
    return "";
}
void
AnalysisResult::addValue(const RegisteredField* field, const std::string& val) {
    // make sure the field is not stored more often than allowed
    if (!p->checkCardinality(field)) {
	return;
    }
    if (checkUtf8(val)) {
        p->m_writer.addValue(this, field, val);
    } else {
        Latin1Converter::lock();
        char* d;
        int32_t len = Latin1Converter::fromLatin1(d, val.c_str(),
                                                    (int32_t)val.length());
        if (len && checkUtf8(d, len)) {
            p->m_writer.addValue(this, field, (const unsigned char*)d, len);
        } else {
            fprintf(stderr, "'%s' is not a UTF8 or latin1 string\n",
                val.c_str());
        }
        Latin1Converter::unlock();
    }
}
void
AnalysisResult::addValue(const RegisteredField* field,
        const char* data, uint32_t length) {
    // make sure the field is not stored more often than allowed
    if (!p->checkCardinality(field)) {
	return;
    }
    if (checkUtf8(data, length)) {
        p->m_writer.addValue(this, field, (const unsigned char*)data, length);
    } else {
        Latin1Converter::lock();
        char* d;
        int32_t len = Latin1Converter::fromLatin1(d, data, length);
        if (len && checkUtf8(d, len)) {
            p->m_writer.addValue(this, field, (const unsigned char*)d, len);
        } else {
            fprintf(stderr, "'%.*s' is not a UTF8 or latin1 string\n",
                length, data);
        }
        Latin1Converter::unlock();
    }
}
void
AnalysisResult::addValue(const RegisteredField* field, int32_t value) {
    if (!p->checkCardinality(field))
	return;
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, uint32_t value) {
    if (!p->checkCardinality(field))
	return;
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, double value) {
    if (!p->checkCardinality(field))
	return;
    p->m_writer.addValue(this, field, value);
}
void
AnalysisResult::addTriplet(const std::string& subject, const std::string& predicate,
        const std::string& object){
    p->m_writer.addTriplet(subject, predicate, object);
}
std::string
AnalysisResult::newAnonymousUri(){
    std::string result;
    result.resize(6);
    result[0]=':';
    for(int i=1; i<6; i++)
      result[i]=(char)((rand() % 26) + 'a');
    return result;
}
bool
AnalysisResult::Private::checkCardinality(const RegisteredField* field) {
    std::map<const Strigi::RegisteredField*, int>::const_iterator i
        = m_occurrences.find(field);
    if (i != m_occurrences.end()) {
	if (i->second >= field->properties().maxCardinality()
                && field->properties().maxCardinality() >= 0) {
	    fprintf(stderr, "%s hit the maxCardinality limit (%d)\n",
		field->properties().name().c_str(),
                    field->properties().maxCardinality());
	    return false;
	} else {
	    m_occurrences[field]++;
	}
    } else {
	m_occurrences[field] = 1;
    }
    return true;
}

