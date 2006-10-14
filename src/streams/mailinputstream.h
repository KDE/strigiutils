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
#ifndef MAILINPUTSTREAM_H
#define MAILINPUTSTREAM_H

#include "substreamprovider.h"

/**
 * This is a proof of concept implementation for handling email streams as
 * archives. It allows one to read the email body and email attachements as
 * streams.
 **/

namespace jstreams {
class SubInputStream;
class StringTerminatedSubStream;
class MailInputStream : public SubStreamProvider {
private:
    // variables that record the current read state
    bool eol; // true if the buffer contains a line end
    int32_t entrynumber;
    int64_t bufstartpos;
    int64_t bufendpos;
    int linenum;
    int maxlinesize;
    const char* linestart;
    const char* lineend;
    const char* bufstart;
    const char* bufend;

    StringTerminatedSubStream* substream;
    std::string subject;
    std::string contenttype;
    std::string contenttransferencoding;
    std::string contentdisposition;
    std::string* lastHeader;

    std::string boundary;

    void readLine();
    void fillBuffer();
    void skipHeader();
    void scanBody();
    void handleHeaderLine();
    void handleBodyLine();
    bool lineIsEndOfBlock();
    void rewindToLineStart();
    bool checkHeaderLine() const;
    void clearHeaders();
    void ensureFileName();
    std::string getValue(const char* n, const std::string& headerline) const;
public:
    explicit MailInputStream(StreamBase<char>* input);
    ~MailInputStream();
    StreamBase<char>* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(StreamBase<char>* input) {
        return new MailInputStream(input);
    }
    const std::string& getSubject() { return subject; }
    const std::string& getContentType() { return contenttype; }
};

} // end namespace jstreams

#endif
