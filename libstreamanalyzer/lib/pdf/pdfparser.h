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
#ifndef PDFPARSER_H
#define PDFPARSER_H

#include <strigi/streambase.h>

class PdfParser {
public:
    class StreamHandler {
    public:
        virtual ~StreamHandler() {}
        virtual Strigi::StreamStatus handle(Strigi::StreamBase<char>* s) = 0;
    };
    class DefaultStreamHandler : public StreamHandler {
    public:
        Strigi::StreamStatus handle(Strigi::StreamBase<char>* s);
    };
    class TextHandler {
    public:
        virtual ~TextHandler() {}
        virtual Strigi::StreamStatus handle(const std::string& s) = 0;
    };
    class DefaultTextHandler : public TextHandler {
        Strigi::StreamStatus handle(const std::string& s);
    };
private:
    const char* start;
    const char* end;
    const char* pos;
    int64_t bufferStart;
    Strigi::StreamBase<char>* stream;
    std::string m_error;

    // parsed objects
    double lastNumber;
    std::string lastName;
    std::string lastOperator;
    std::string lastString;
    void* lastObject;

    // event handlers
    StreamHandler* streamhandler;
    TextHandler* texthandler;

    Strigi::StreamStatus read(int32_t min, int32_t max);
    void forwardStream(Strigi::StreamBase<char>* s);
    Strigi::StreamStatus checkForData(int32_t m);
    bool isInString(char c, const char* s, int32_t n);
    Strigi::StreamStatus skipFromString(const char*str, int32_t n);
    Strigi::StreamStatus skipNotFromString(const char*str, int32_t n);
    Strigi::StreamStatus skipWhitespaceOrComment();
    Strigi::StreamStatus skipWhitespace();
    Strigi::StreamStatus skipKeyword(const char* str, int32_t len);
    Strigi::StreamStatus skipXRef();
    Strigi::StreamStatus skipTrailer();
    Strigi::StreamStatus skipXChars();
    Strigi::StreamStatus skipDigits();
    Strigi::StreamStatus skipStartXRef();
    Strigi::StreamStatus skipNumber();
    Strigi::StreamStatus parseObjectStreamObject(int nestDepth);
    Strigi::StreamStatus parseContentStreamObject();
    Strigi::StreamStatus parseComment();
    Strigi::StreamStatus parseBoolean();
    Strigi::StreamStatus parseNumber();
    Strigi::StreamStatus parseNumberOrIndirectObject();
    Strigi::StreamStatus parseLiteralString();
    Strigi::StreamStatus parseHexString();
    Strigi::StreamStatus parseName();
    Strigi::StreamStatus parseOperator();
    Strigi::StreamStatus parseDictionaryOrStream();
    Strigi::StreamStatus parseArray(int nestDepth);
    Strigi::StreamStatus parseNull();
    Strigi::StreamStatus parseObjectStreamObjectDef();

    Strigi::StreamStatus handleSubStream(Strigi::StreamBase<char>* s,
        const std::string& type, int32_t offset, int32_t n);
    Strigi::StreamStatus handleSubStream(Strigi::StreamBase<char>* s,
        const std::string& type, int32_t offset, int32_t n, bool hasfilter,
        const std::string& filter);
    Strigi::StreamStatus parseObjectStream(Strigi::StreamBase<char>*,
        int32_t offset, int32_t n);
    Strigi::StreamStatus parseContentStream(Strigi::StreamBase<char>*);
public:
    PdfParser();
    Strigi::StreamStatus parse(Strigi::StreamBase<char>* s);
    const std::string& error() { return m_error; }
    void setStreamHandler(StreamHandler* handler) { streamhandler = handler; }
    void setTextHandler(TextHandler* handler) { texthandler = handler; }
};

#endif
