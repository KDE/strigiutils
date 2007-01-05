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
#ifndef XMLINDEXWRITER_H
#define XMLINDEXWRITER_H

#include "indexwriter.h"
#include "indexable.h"
#include <iostream>
#include <map>

class XmlIndexWriter : public jstreams::IndexWriter {
private:
    struct Data {
         std::multimap<std::string, std::string> values;
         std::string text;
    };

    std::ostream& out;

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
            } else if (isspace(c)) {
                if (!lastwhite) {
                    out.put(c);
                }
                lastwhite = true;
            } else {
                lastwhite = false;
                out.put(c);
            }
            p++;
        }
    }
    void escape(std::string& value) {
        int namp, nlt, ngt, napos;
        namp = nlt = ngt = napos = 0;
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
            } else if (c <= 8) {
                value = "";
                return;
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
        if (!(namp||nlt||ngt|napos)) return;

        std::string ov(value);
        p = ov.c_str();
        end = p + ov.size();
        int newsize = value.size()+4*namp+3*(nlt+ngt)+5*napos;
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
            } else if (c <= 8) {
                value = "";
                return;
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
    void startIndexable(jstreams::Indexable* idx) {
        void* m = new Data();
        idx->setWriterData(m);
    }
    void finishIndexable(const jstreams::Indexable* idx) {
        Data* d = static_cast<Data*>(idx->getWriterData());
        std::string v = idx->getPath();
        escape(v);
        out << " <file uri='" << v << "' mtime='" << (int)idx->getMTime()
            << "'>\n";

        if (idx->getMimeType().size()) {
            v = idx->getMimeType();
            escape(v);
            out << " <value name='mimetype'>" << v << "</value>\n";
        }
        if (idx->getEncoding().size()) {
            v = idx->getEncoding();
            escape(v);
            out << " <value name='encoding'>" << v << "</value>\n";
        }

        std::multimap<std::string, std::string>::iterator i, end;
        end = d->values.end();
        for (i=d->values.begin(); i!=end; ++i) {
            escape(i->second);
            out << "  <value name='" << i->first << "'>" << i->second
                << "</value>\n";
        }
        out << "  <value name='depth'>" << (int)idx->getDepth() << "</value>\n";
        if (d->text.size() > 0) {
            out << "  <text>";
            printText(d->text);
            out << "</text>\n";
        }
        out << " </file>\n";
        delete d;
    }
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length) {
        Data* d = static_cast<Data*>(idx->getWriterData());
        d->text.append(text, length);
    }
    void setField(const jstreams::Indexable* idx, const std::string &fieldname,
            const std::string& value) {
        Data* d = static_cast<Data*>(idx->getWriterData());
        d->values.insert(
            std::make_pair<std::string,std::string>(fieldname, value));
    }
public:
    explicit XmlIndexWriter(std::ostream& o) :out(o) {
    }
    ~XmlIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};

#endif
