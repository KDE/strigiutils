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
#ifndef RDFINDEXWRITER_H
#define RDFINDEXWRITER_H

#include <strigi/indexwriter.h>
#include <strigi/indexmanager.h>
#include <strigi/analysisresult.h>
#include "tagmapping.h"
#include <strigi/fieldtypes.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/strigi_thread.h>
#include <iostream>
#include <sstream>
#include <map>
#include <list>

typedef std::map<std::string, std::map<std::string, std::list<std::string> > > rdfset;

class RdfIndexWriter : public Strigi::IndexWriter {
private:
    struct Data {
         std::multimap<const Strigi::RegisteredField*, std::string> values;
         std::string text;
    };
    std::map<STRIGI_THREAD_TYPE, std::vector<Data*> > data;
    struct Tag {
         std::string open;
         std::string close;
         int refcount;
    };

    STRIGI_MUTEX_DEFINE(mutex);
    std::ostream& out;

    rdfset& rdf;

    const TagMapping& mapping;

    void printText(const std::string& text) {
        const char* p = text.c_str();
        const char* end = p + text.size();
        char nb = 0;
        bool lastwhite = true;
        while (p < end) {
            char c = *p;
            if (nb) {
                if ((0xC0 & c) != 0x80) {
                    return;
                }
                out.put(c);
                nb--;
            } else if ((0xE0 & c) == 0xC0) {
                nb = 1;
                out.put(c);
            } else if ((0xF0 & c) == 0xE0) {
                nb = 2;
                out.put(c);
            } else if ((0xF8 & c) == 0xF0) {
                nb = 3;
                out.put(c);
            } else if (c <= 8) {
                return;
            } else if (c == '&') {
                out << "&amp;";
            } else if (c == '<') {
                out << "&lt;";
            } else if (c == '>') {
                out << "&gt;";
            } else if (isspace(c) != 0) {
                // we've to handle dos formatting
                //'\r' char is ignored, it isn't wroten to out and doesn't
                //change lastwhite value (so the following '\n' will be handled)
                if (!lastwhite && (c!= '\r')) {
                    out.put(c);
                    lastwhite = true;
                }
            } else {
                lastwhite = false;
                out.put(c);
            }
            p++;
        }
    }
    static void escape(std::string& value) {
        int namp, nlt, ngt, napos, nexcept;
        namp = nlt = ngt = napos = nexcept = 0;
        const char* p = value.c_str();
        const char* end = p + value.size();
        char nb = 0;
        while (p < end) {
            char c = *p;
            if (nb) {
                if ((0xC0 & c) != 0x80) {
                    value = "";
                    return;
                }
                nb--;
            } else if ((0xE0 & c) == 0xC0) {
                nb = 1;
            } else if ((0xF0 & c) == 0xE0) {
                nb = 2;
            } else if ((0xF8 & c) == 0xF0) {
                nb = 3;
            } else if (c < 32 && c != 9 && c != 10 && c != 12) {
                nexcept++;
            } else if (c == '&') {
                namp++;
            } else if (c == '<') {
                nlt++;
            } else if (c == '>') {
                ngt++;
            } else if (c == '\'') {
                napos++;
            }
            p++;
        }
        // if no character has to be escaped, just return
        if (!(namp||nlt||ngt|napos|nexcept)) {
            return;
        }

        std::string ov(value);
        p = ov.c_str();
        end = p + ov.size();
        int newsize = (int)value.size()+4*namp+3*(nlt+ngt)+5*napos+3*nexcept;
        value.clear();
        value.reserve(newsize);
        while (p < end) {
            char c = *p;
            if (nb) {
                if ((0xC0 & c) != 0x80) {
                    value = "";
                    return;
                }
                nb--;
                value += c;
            } else if ((0xE0 & c) == 0xC0) {
                nb = 1;
                value += c;
            } else if ((0xF0 & c) == 0xE0) {
                nb = 2;
                value += c;
            } else if ((0xF8 & c) == 0xF0) {
                nb = 3;
                value += c;
            } else if (c < 32 && c != 9 && c != 10 && c != 12) {
                char s[4];
                snprintf(s, 4, "%%%2x", (unsigned char)c);
                value += s;
            } else if (c == '&') {
                value += "&amp;";
            } else if (c == '<') {
                value += "&lt;";
            } else if (c == '>') {
                value += "&gt;";
            } else if (c == '\'') {
                value += "&apos;";
            } else {
                value += c;
            }
            p++;
        }
    }
protected:
    void startAnalysis(const Strigi::AnalysisResult* ar) {
        STRIGI_MUTEX_LOCK(&mutex);
        std::vector<Data*>& dv = data[STRIGI_THREAD_SELF()];
        STRIGI_MUTEX_UNLOCK(&mutex);
        unsigned char depth = ar->depth();
        if (depth >= dv.size()) {
            dv.push_back(new Data());
        }
        Data* d = dv[depth];
        ar->setWriterData(d);
    }
    void printValue(const Strigi::AnalyzerConfiguration& config,
            const Strigi::RegisteredField* name, std::string& value) {
        if (config.indexType(name) != Strigi::AnalyzerConfiguration::None) {
            const Tag* tag = static_cast<const Tag*>(name->writerData());
            escape(value);
            out << tag->open << value << tag->close;
        }
    }
    void finishAnalysis(const Strigi::AnalysisResult* ar) {
        STRIGI_MUTEX_LOCK(&mutex);
        Data* d = static_cast<Data*>(ar->writerData());
        const Strigi::AnalyzerConfiguration& config = ar->config();
        //const Strigi::FieldRegister& fr = config.fieldRegister();
        std::string v = ar->path();
        escape(v);
/*        out << " <" << mapping.map("file") << " " << mapping.map("uri")
            << "='" << v << "' " << mapping.map("mtime") << "='"
            << (int)ar->mTime()
            << "'>\n";

        if (ar->encoding().size()) {
            v.assign(ar->encoding());
            printValue(config, fr.encodingField, v);
        }

        std::multimap<const Strigi::RegisteredField*, std::string>::iterator
            i, end;
        end = d->values.end();
        for (i = d->values.begin(); i != end; ++i) {
            printValue(config, i->first, i->second);
        }
        std::ostringstream oss;
        oss << (int)ar->depth();
        v = oss.str();
        printValue(config, fr.embeddepthField, v);
        if (d->text.size() > 0) {
            out << "  <text>";
            printText(d->text);
            out << "</text>\n";
        }
        out << " </" << mapping.map("file") << ">\n";
*/
	STRIGI_MUTEX_UNLOCK(&mutex);

	std::string subj = d->values.find(config.fieldRegister().pathField)->second;

	for (std::multimap<const Strigi::RegisteredField*, std::string>::iterator i = d->values.begin();
	      i != d->values.end(); i++) {
            addTriplet(subj, i->first->key(), i->second);
        }
	if (!d->text.empty())
	    addTriplet(subj,"http://www.semanticdesktop.org/ontologies/2007/01/19/nie#plainTextContent",d->text);
	
        d->values.clear();
        d->text.assign("");
    }
    void addText(const Strigi::AnalysisResult* ar, const char* text,
        int32_t length) {
        Data* d = static_cast<Data*>(ar->writerData());
        if (d->text.size() < 10000000) {
            d->text.append(text, length);
            d->text.append("\n");
        }
    }
    void addValue(const Strigi::AnalysisResult* ar,
            const Strigi::RegisteredField* field, const std::string& value) {
        Data* d = static_cast<Data*>(ar->writerData());
        d->values.insert(std::make_pair(field, value));
    }
    void addValue(const Strigi::AnalysisResult* ar,
            const Strigi::RegisteredField* field,
            const unsigned char* data, uint32_t size) {
        Data* d = static_cast<Data*>(ar->writerData());
        d->values.insert(
            std::make_pair(field, std::string((const char*)data, size)));
    }
    void addValue(const Strigi::AnalysisResult* ar,
            const Strigi::RegisteredField* field, uint32_t value) {
        Data* d = static_cast<Data*>(ar->writerData());
        static std::ostringstream v;
        v.str("");
        v << value;
        d->values.insert(std::make_pair(field, v.str()));
    }
    void addValue(const Strigi::AnalysisResult* ar,
            const Strigi::RegisteredField* field, int32_t value) {
        Data* d = static_cast<Data*>(ar->writerData());
        static std::ostringstream v;
        v.str("");
        v << value;
        d->values.insert(std::make_pair(field, v.str()));
    }
    void addValue(const Strigi::AnalysisResult* ar,
            const Strigi::RegisteredField* field, double value) {
        Data* d = static_cast<Data*>(ar->writerData());
        static std::ostringstream v;
        v.str("");
        v << value;
        d->values.insert(std::make_pair(field, v.str()));
    }
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {
	STRIGI_MUTEX_LOCK(&mutex);
	rdf[subject][predicate].push_back(object);
        STRIGI_MUTEX_UNLOCK(&mutex);
    }
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* field, const std::string& name,
        const std::string& value) {}
    void initWriterData(const Strigi::FieldRegister&);
    void releaseWriterData(const Strigi::FieldRegister&);
public:
    explicit RdfIndexWriter(std::ostream& o, const TagMapping& m, rdfset& r)
            :out(o), rdf(r), mapping(m) {
        STRIGI_MUTEX_INIT(&mutex);
    }
    ~RdfIndexWriter() {
         std::map<STRIGI_THREAD_TYPE, std::vector<Data*> >::const_iterator j;
         for (j = data.begin(); j != data.end(); ++j) {
             std::vector<Data*>::const_iterator i;
             for (i = j->second.begin(); i != j->second.end(); ++i) {
                 delete *i;
             }
         }
         STRIGI_MUTEX_DESTROY(&mutex);
    }
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};

class RdfIndexManager : public Strigi::IndexManager {
private:
    RdfIndexWriter writer;
public:
    RdfIndexManager(std::ostream& o, const TagMapping& m, rdfset& r) :writer(o, m, r) {}
    Strigi::IndexWriter* indexWriter() {
        return &writer;
    }
    Strigi::IndexReader* indexReader() {
        return 0;
    }
};

#endif
