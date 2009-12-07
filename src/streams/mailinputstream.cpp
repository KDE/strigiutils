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
# include "config.h"
#endif


#include "mailinputstream.h"
#include "subinputstream.h"
#include "stringterminatedsubstream.h"
#include "base64inputstream.h"
#include "iconv.h"
#include <cstring>
#include <sstream>
#include <iostream>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

using namespace std;
using namespace Strigi;

char
decodeHex(char h) {
    if (h >= 'A' && h <= 'F') return (char)(10+h-'A');
    if (h >= 'a' && h <= 'f') return (char)(10+h-'a');
    return (char)(h - '0');
}

class Decoder {
private:
    char* buffer;
    size_t bufferlen;
    map<string, iconv_t> iconvs;
public:
    Decoder() :buffer(0), bufferlen(0) {}
    ~Decoder() {
        free(buffer);
        map<string, iconv_t>::const_iterator i;
        for (i = iconvs.begin(); i != iconvs.end(); ++i) {
            iconv_close(i->second);
        }
    }
    void decode(const string& enc, string& data);
};
void
Decoder::decode(const string& enc, string& data) {
    iconv_t conv;
    if (iconvs.find(enc) == iconvs.end()) {
        conv = iconvs[enc] = iconv_open("UTF-8", enc.c_str());
    } else {
        conv = iconvs[enc];
    }
    if (conv == (iconv_t)-1) return;
    ICONV_CONST char* in = (char*)data.c_str();
    size_t ilen = data.length();
    size_t olen = 4*ilen;
    if (olen > bufferlen) {
        bufferlen = olen;
        buffer = (char*)realloc(buffer, bufferlen);
    }
    if (olen > 0) {
        char* out = buffer;
        char* mem = out;
        size_t r = iconv(conv, &in, &ilen, &out, &olen);
        if (r != (size_t)-1) {
            data.assign(mem, out-mem);
        }
    }
}

class QuotedPrintableDecoder {
private:
    string decoded;
public:
    string& decodeQuotedPrintable(const char* v, uint32_t len);
};
class HeaderDecoder : public QuotedPrintableDecoder, Decoder {
private:
    string decoded;
public:
    const string& decodedHeaderValue(const char* v, uint32_t len);
};

string&
QuotedPrintableDecoder::decodeQuotedPrintable(const char* v, uint32_t len) {
    if (decoded.size() < len) {
        decoded.reserve(len);
    }
    decoded.resize(0);
    const char* pos = v;
    const char* end = v + len;
    char c;
    while (v < end) {
        if (*v == '=' && end - v > 2 && isxdigit(v[1]) && isxdigit(v[2])) {
            decoded.append(pos, v - pos);
            c = (char)(decodeHex(v[1])*16 + decodeHex(v[2]));
            decoded.append(&c, 1);
            pos = v = v + 3;
        } else if (*v == '_') {
            decoded.append(pos, v - pos);
            decoded.append(" ");
            pos = v = v + 1;
        } else {
            v++;
        }
    }
    if (pos < end) {
        decoded.append(pos, end-pos);
    }
    return decoded;
}
/**
 * This function can decode a mail header if it contains utf8 encoded in base64.
 **/
const string&
HeaderDecoder::decodedHeaderValue(const char* v, uint32_t len) {
    if (decoded.size() < len) {
        decoded.reserve(len*2);
    }
    decoded.resize(0);
    const char* s = v;
    const char* p = v;
    const char* e = s + len;
    while (s < e) {
        if (e-s > 8 && *s == '=' && s[1] == '?') {
            // start of encoded data, find the next position of '?','?' and '?='
            const char *q1, *q2, *end;
            q1 = s+2;
            while (q1 < e && *q1 != '?') q1++;
            q2 = q1+1;
            while (q2 < e && *q2 != '?') q2++;
            end = q2+1;
            while (end < e && *end != '?') end++;
            if (e - end < 1 || end[1] != '=') {
                s++;
                continue;
            }
            // save the stuff from before the encoding
            decoded.append(p, s-p);
            s += 2;
            q1++;
            q2++;
            // find the end
            if (*q1 == 'b' || *q1 == 'B') {
                string str(Base64InputStream::decode(q2, end-q2));
                if (strncasecmp("utf-8", s, 5)) {
                    string encoding(s, q1-s-1);
                    decode(encoding, str);
                }
                decoded.append(str);
            } else if (*q1 == 'q' || *q1 =='Q') {
                string& str(decodeQuotedPrintable(q2, (char)(end-q2)));
                if (strncasecmp("utf-8", s, 5) != 0) {
                    string encoding(s, q1-s-1);
                    decode(encoding, str);
                }
                decoded.append(str);
            } else {
                s -= 1;
            }

            // continue after the quoted data
            s = p = end + 2;
        } else if (e-s > 3 && s[0] == 13 && s[1] == 10
                && (s[2] == 9 || s[2] == 32)) {
            // skip the CRLF WSP used for folding the header lines
            decoded.append(p, s-p);
            s = p = s + 4;
        } else {
            s++;
        }
    }
    if (p < e) {
        decoded.append(p, e-p);
    }
    return decoded;
}
bool
checkHeaderKey(const char* data, int32_t left) {
    if (left >= 9 && strncasecmp("Received:", data, 9) == 0) {
        return true;
    }
    if (left >= 5 && strncasecmp("From:", data, 5) == 0) {
        return true;
    }
    return false;
}

/**
 * Validate a mail header. The header format is checked, but not the presence
 * of required fields. It is recommended to use a datasize of at least 512
 * bytes.
 **/
bool
MailInputStream::checkHeader(const char* data, int32_t datasize) {
    // the fileheader should contain a required header and have at least 5
    // header lines
    // 'Received' or 'From' (case insensitive)
    int linecount = 1;
    bool key = true;
    bool slashr = false;
    int32_t pos = 0;
    bool reqheader = checkHeaderKey(data, datasize);
    char prevc = 0;
    while (pos < datasize) {
        unsigned char c = data[pos++];
        if (slashr) {
            slashr = false;
            if (c == '\n') {
                if (!reqheader) {
                    reqheader = checkHeaderKey(data+pos, datasize-pos);
                }
                continue;
            }
        }
        if (key) {
            if (c == ':' || (isblank(c) && isspace(prevc))) {
                // ':' signals the end of the key, a line starting with space
                // is a continuation of the previous line's value
                key = false;
            } else if ((c == '\n' || c == '\r') && reqheader && linecount >= 5
                    && (prevc == '\n' || prevc == '\r')) {
                // if at least 5 header lines were read and an empty line is
                // encountered, the mail header is valid
                return true;
            } else if (c != '-' && c != '.' && c != '_' && !isalnum(c)
			    && c != '#') {
                // an invalid character in the key
                return false;
            }
        } else {
            // check that the text is 7-bit
            if (c == '\n' || c == '\r') {
                // a new line starts, so a new key
                key = true;
                linecount++;
                // enable reading of \r\n line endings
                if (c == '\r') {
                    slashr = true;
                } else if (!reqheader) {
                    reqheader = checkHeaderKey(data+pos, datasize-pos);
                }
            }
        }
        prevc = c;
    }
    return reqheader && linecount >= 5;
}
class MailInputStream::Private {
public:
    MailInputStream* const m;
    int64_t nextLineStartPosition;
    // variables that record the current read state
    int32_t entrynumber;
    int maxlinesize;
    const char* linestart;
    const char* lineend;

    StringTerminatedSubStream* substream;
    std::string m_contenttransferencoding;
    std::string m_contentdisposition;

    std::stack<std::string> boundary;

    HeaderDecoder decoder;

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

    Private(MailInputStream* mail);
    ~Private();
};
MailInputStream::Private::Private(MailInputStream* mail) :m(mail) {
    substream = 0;
    entrynumber = 0;
    nextLineStartPosition = 0;
}
MailInputStream::Private::~Private() {
    if (substream && substream != m->m_entrystream) {
        delete substream;
    }
}
MailInputStream::MailInputStream(InputStream* input)
        : SubStreamProvider(input), p(new Private(this)) {
    // parse the header and store the imporant header fields
    p->readHeader();
    if (m_status != Ok) {
        fprintf(stderr, "no valid header\n");
        return;
    }
}
MailInputStream::~MailInputStream() {
    delete p;
}
/**
 * This function read the input until the end of a header line.
 * A header line can span multiple normal lines. All normal lines after the
 * first normal line start with a whitespace.
 * This means a header line is ended by on of these patterns (where \S is
 *  a non-whitespace character).
 *   '\r\S', '\n\S', '\r\n\S', '\r\r', '\n\n', '\r\n\r'
 **/
void
MailInputStream::Private::readHeaderLine() {
    // state: 0 -> ok, 1 -> '\r', 2 -> '\n', 3 -> '\r\n'
    char state = 0;
    int32_t nread;
    int32_t linepos = 0;
    bool completeLine = false;
    char c = 0;

    m->m_input->reset(nextLineStartPosition);
    do {
        nread = m->m_input->read(linestart, linepos+1, maxlinesize);
        if (nread < linepos+1) {
            completeLine = true;
            lineend = linestart + nread;
            m->m_status = Eof;
            return;
        }
        m->m_input->reset(nextLineStartPosition);
        if (m->m_input->status() == Error) {
            m->m_status = Error;
            m->m_error = m->m_input->error();
            return;
        } else if (linepos >= maxlinesize) {
            // error line is too long
            m->m_status = Error;
            ostringstream out;
            out << "mail header line is too long: more than " << linepos
                << " bytes.";
            m->m_error = out.str();
            return;
        } else {
            while (linepos < nread) {
                c = linestart[linepos];
                if (state == 0) {
                    if (c == '\r') {
                        state = 1;
                    } else if (c == '\n') {
                        state = 2;
                    }
                } else if (state == 1) { // '\r'
                    if (c == '\n') {
                        state = 3;
                    } else if (c == '\r' || !isspace(c)) { // end
                        completeLine = true;
                        lineend = linestart + linepos - 1;
                        break;
                    } else {
                        state = 0;
                    }
                } else if (state == 2) { // '\n'
                    if (c == '\n' || !isspace(c)) { // end
                        completeLine = true;
                        lineend = linestart + linepos - 1;
                        break;
                    } else {
                        state = 0;
                    }
                } else { // state == 3   '\r\n'
                    if (c == '\r' || linepos == 2 || !isspace(c)) { // end
                        completeLine = true;
                        lineend = linestart + linepos - 2;
                        break;
                    } else {
                        state = 0;
                    }
                }
                linepos++;
            }
        }
    } while (!completeLine);
    nextLineStartPosition += linepos;
}
string
MailInputStream::Private::value(const char* n, const string& headerline) const {
    size_t nl = strlen(n);
    string value;
    // get the value
    const char* hl = headerline.c_str();
    const char* v = strcasestr(hl, n);
    if (v == 0) {
        // so far we just scan for a value attribute
        return value;
    }
    v += nl;
    v += strspn(v, "= \n\r");
    const char* vend = strchr(v, ';');
    if (vend == 0) {
        vend = hl + headerline.length();
    }
    if (*v == '"' && vend-v > 2) {
        value.assign(v+1, vend-v-2);
    } else {
        value.assign(v, vend-v);
    }
    return value;
}
void
MailInputStream::Private::readHeader() {
    maxlinesize = 1024*1024;

    readHeaderLine();
    while (m->m_status == Ok && linestart != lineend) {
        handleHeaderLine();
        readHeaderLine();
    }
}
/**
 * Read lines from the email until a line contains the boundary.
 * If a boundary is encountered, the block header is parsed.
 **/
void
MailInputStream::Private::scanBody() {
    while (m->m_status == Ok) {
        readHeaderLine();
        int32_t len = (int32_t)(lineend - linestart);
        if (len > 2 && strncmp("--", linestart, 2) == 0) {
            int32_t blen = (int32_t)boundary.top().length();
            if (len == blen + 4 && strncmp(linestart + 2 + blen, "--", 2) == 0
                    && strncmp(linestart + 2, boundary.top().c_str(), blen)
                        == 0) {
                // check if this is the end of a multipart
                boundary.pop();
                if (boundary.size() == 0) {
                    m->m_status = Eof;
                }
            } else if (len == blen + 2
                    && strncmp(linestart + 2, boundary.top().c_str(), blen)
                        == 0) {
                if (handleBodyLine()) {
                    break;
                }
            }
        }
    }
}
void
MailInputStream::Private::handleHeaderLine() {
    static const char* subject = "Subject:";
    static const char* contenttype = "Content-Type:";
    static const char* to = "To:";
    static const char* from = "From:";
    static const char* cc = "Cc:";
    static const char* bcc = "Bcc:";
    static const char* messageid = "Message-ID:";
    static const char* inreplyto = "In-Reply-To:";
    static const char* references = "References:";
    static const char* contenttransferencoding = "Content-Transfer-Encoding:";
    static const char* contentdisposition = "Content-Disposition:";
    int32_t len = (int32_t)(lineend - linestart);
    if (len < 2) return;
    if (len < 8) {
        return;
    } else if (strncasecmp(linestart, subject, 8) == 0) {
        int32_t offset = 8;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_subject = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, to, 3) == 0) {
        int32_t offset = 3;
        // FIXME: should split for ','
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_to = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, from, 5) == 0) {
        int32_t offset = 5;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_from = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, cc, 3) == 0) {
        int32_t offset = 3;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_cc = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, bcc, 4) == 0) {
        int32_t offset = 4;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_bcc = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, messageid, 11) == 0) {
        int32_t offset = 11;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_messageid = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, inreplyto, 12) == 0) {
        int32_t offset = 12;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_inreplyto = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, references, 11) == 0) {
        int32_t offset = 11;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_references = decoder.decodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, contenttype, 13) == 0) {
        int32_t offset = 13;
        while (offset < len && isspace(linestart[offset])) offset++;
        m->m_contenttype = std::string(linestart+offset, len-offset);
        // get the boundary
        string b = value("boundary", m->m_contenttype);
        if (b.size()) {
            boundary.push(b);
        }
    } else if (strncasecmp(linestart, contenttransferencoding, 26) == 0) {
        m_contenttransferencoding = std::string(linestart, len);
    } else if (strncasecmp(linestart, contentdisposition, 20) == 0) {
        m_contentdisposition = std::string(linestart, len);
    }
}
bool
MailInputStream::Private::checkHeaderLine() const {
    assert(lineend - linestart >= 0);
    bool validheader = linestart < lineend;
    if (validheader) {
        const char* colpos = linestart;
        while (*colpos != ':' && ++colpos != lineend) {}
        validheader = colpos != lineend || isblank(*linestart);
    }
    return validheader;
}
/**
 * Handle the body part header.
 **/
bool
MailInputStream::Private::handleBodyLine() {
    clearHeaders();

    // start of new block
    // read part header
    bool validheader;
    size_t n = boundary.size();
    do {
        readHeaderLine();
        validheader = m->m_status == Ok && checkHeaderLine();
        if (validheader) {
            handleHeaderLine();
        }
    } while (m->m_status == Ok && validheader);
    if (boundary.size() > n) {
        return false;
    }
    readHeaderLine();
    if (m->m_status != Ok) {
        return false;
    }

    // get the filename
    m->m_entryinfo.filename = value("filename", m_contentdisposition);
    if (m->m_entryinfo.filename.length() == 0) {
        m->m_entryinfo.filename = value("name", m->m_contenttype);
    }

    // create a stream that's limited to the content
    substream = new StringTerminatedSubStream(m->m_input, "--"+boundary.top());
    // set a reasonable buffer size
    if (strcasestr(m_contenttransferencoding.c_str(), "base64")) {
        m->m_entrystream = new Base64InputStream(substream);
    } else {
        m->m_entrystream = substream;
    }
    return true;
}
/**
 * Not all parts of multipart emails have a name. This function keeps a running
 * number to make sure all parts have a name so that they can be referenced
 * later.
 **/
void
MailInputStream::Private::ensureFileName() {
    entrynumber++;
    if (m->m_entryinfo.filename.length() == 0) {
        ostringstream o;
        o << entrynumber;
        m->m_entryinfo.filename = o.str();
    }
    m->m_entryinfo.type = EntryInfo::File;
}
InputStream*
MailInputStream::nextEntry() {
    if (m_status != Ok) return 0;
    // if the mail does not consist of multiple parts, we give a pointer to
    // the input stream
    if (p->boundary.size() == 0) {
        // signal eof because we only return eof once
        m_status = Eof;
        m_entrystream = new SubInputStream(m_input);
        m_entryinfo.filename = "body";
        return m_entrystream;
    }
    // read anything that's left over in the previous stream
    if (p->substream) {
        const char* dummy;
        while (p->substream->status() == Ok) {
            p->substream->read(dummy, 1, 0);
        }
        if (p->substream->status() == Error) {
            m_status = Error;
        } else {
            p->nextLineStartPosition = p->substream->offset()
                + p->substream->size();
        }
        if (p->substream && p->substream != m_entrystream) {
            delete p->substream;
        }
        p->substream = 0;
        delete m_entrystream;
        m_entrystream = 0;

        if (m_status != Ok) {
            return 0;
        }
    }
    p->scanBody();

    if (m_entrystream == 0) {
        m_status = Eof;
    }
    p->ensureFileName();
    return m_entrystream;
}
void
MailInputStream::Private::clearHeaders() {
    m->m_contenttype.resize(0);
    m_contenttransferencoding.resize(0);
    m_contentdisposition.resize(0);
}
