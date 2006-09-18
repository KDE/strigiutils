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
#include <map>

class XmlIndexWriter : public jstreams::IndexWriter {
private:
    struct Data {
         std::multimap<std::string, std::string> values;
         std::string text;
    };

    FILE* fd;

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
                nb--;
            } else if ((0xE0 & c) == 0xC0) {
                nb = 1;
            } else if ((0xF0 & c) == 0xE0) {
                nb = 2;
            } else if ((0xF8 & c) == 0xF0) {
                nb = 3;
            } else if (c <= 8) {
                return;
            } else if (c == '&') {
                fprintf(fd, "&amp;");
            } else if (c == '<') {
                fprintf(fd, "&lt;");
            } else if (c == '>') {
                fprintf(fd, "&gt;");
            } else if (isspace(c)) {
                if (!lastwhite) {
                    fwrite(&c, 1, 1, fd);
                }
                lastwhite = true;
            } else {
                lastwhite = false;
                fwrite(&c, 1, 1, fd);
            }
            p++;
        }
    }
    void escape(std::string& value) {
        int namp, nlt, ngt;
        namp = nlt = ngt = 0;
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
            }
            p++;
        }
        // if no character has to be escaped, just return
        if (!(namp||nlt||ngt)) return;

        std::string ov(value);
        p = ov.c_str();
        end = p + ov.size();
        int newsize = value.size()+4*namp+3*(nlt+ngt);
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
                value += "&amp;";
            } else if (c == '<') {
                value += "&lt;";
            } else if (c == '>') {
                value += "&gt;";
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
        fprintf(fd, " <file uri='%s' mtime='%i'>\n",
            idx->getName().c_str(), (int)idx->getMTime());

        if (idx->getMimeType().size()) {
            fprintf(fd, " <value name='mimetype'>%s</value>\n",
                idx->getMimeType().c_str());
        }
        if (idx->getEncoding().size()) {
            fprintf(fd, " <value name='encoding'>%s</value>\n",
                idx->getEncoding().c_str());
        }

        std::multimap<std::string, std::string>::iterator i, end;
        end = d->values.end();
        for (i=d->values.begin(); i!=end; ++i) {
            escape(i->second);
            fprintf(fd, "  <value name='%s'>%s</value>\n", i->first.c_str(),
                i->second.c_str());
        }
        if (d->text.size()) {
            fprintf(fd, "  <text>");
            printText(d->text);
            fprintf(fd, "</text>\n");
        }
        fprintf(fd, " </file>\n");
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
        d->values.insert(make_pair(fieldname, value));
    }
public:
    XmlIndexWriter() {
        fd = stdout;
    }
    ~XmlIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
};

#endif
