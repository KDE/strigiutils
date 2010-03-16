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
#ifndef STRIGI_MAILINPUTSTREAM_H
#define STRIGI_MAILINPUTSTREAM_H

#include <strigi/streambase.h>
#include <strigi/substreamprovider.h>
#include <stack>

namespace Strigi {

class StringTerminatedSubStream;

/**
 * This is an implementation for handling email streams as
 * archives. It allows one to read the email body and email attachements as
 * streams.
 **/
class STREAMS_EXPORT MailInputStream : public SubStreamProvider {
private:
    class Private;
    int64_t old1;
    // variables that record the current read state
    int32_t old2;
    int old3;
    const char* old4;
    const char* old5;

    Private* const p;
    std::string m_subject;
    std::string m_from;
    std::string m_to;
    std::string m_cc;
    std::string m_bcc;
    std::string m_messageid;
    std::string m_inreplyto;
    std::string m_references;
    std::string m_contenttype;
    std::string old6;
    std::string old7;

    std::stack<std::string> old8;

    void readHeaderLine();
    void readHeader();
    void scanBody();
    void handleHeaderLine();
    bool handleBodyLine();
    bool lineIsEndOfBlock();
    bool checkHeaderLine() const;
    void clearHeaders();
    void ensureFileName();
    std::string value(const char* n, const std::string& headerline) const;
public:
    explicit MailInputStream(InputStream* input);
    ~MailInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new MailInputStream(input);
    }
    const std::string& subject() { return m_subject; }
    const std::string& from() { return m_from; }
    const std::string& to() { return m_to; }
    const std::string& cc() { return m_cc; }
    const std::string& bcc() { return m_bcc; }
    const std::string& messageid() { return m_messageid; }
    const std::string& inreplyto() { return m_inreplyto; }
    const std::string& references() { return m_references; }
    const std::string& contentType() { return m_contenttype; }
};

} // end namespace Strigi

#endif
