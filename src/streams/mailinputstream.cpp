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

#include "jstreamsconfig.h"
#include "mailinputstream.h"
#include "subinputstream.h"
#include "stringterminatedsubstream.h"
#include "base64inputstream.h"
#include <cstring>
#include <sstream>
using namespace jstreams;
using namespace std;

char
decodeHex(char h) {
    if (h >= 'A' && h <= 'F') return 10+h-'A';
    if (h >= 'a' && h <= 'f') return 10+h-'a';
    return h - '0';
}
string
decodeQuotedPrintable(const char* v, int32_t len) {
    string decoded;
    decoded.reserve(len);
    const char* pos = v;
    const char* end = v + len;
    char c;
    while (v < end) {
        if (*v == '=' && end - v > 2 && isxdigit(v[1]) && isxdigit(v[2])) {
            decoded.append(pos, v - pos);
            c = decodeHex(v[1])*16 + decodeHex(v[2]);
            decoded.append(&c, 1);
            pos = v = v + 3;
        } else if (*v == '_') {
            decoded.append(pos, v - pos);
            decoded.append(" ");
            v++;
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
string
getDecodedHeaderValue(const char* v, int32_t len) {
    string decoded;
    decoded.reserve(len*2);
    const char* s = v;
    const char* p = v;
    const char* e = s + len;
    while (s < e) {
        if (*s == '=' && e-s >= 12 && strncasecmp("?utf-8?", s+1, 7) == 0) {
            if (strncasecmp("b?", s+8, 2) == 0) {
                const char* ec = s + 10;
                while (ec < e && *ec != '?') ec += 4;
                if (ec < e - 1) {
                    decoded.append(p, s-p);
                    decoded.append(Base64InputStream::decode(s+10, ec-10-s));
                    s = p = ec + 2;
                } else {
                    s++;
                }
            } else if (strncasecmp("q?", s+8, 2) == 0) {
                const char* ec = s + 10;
                while (ec < e && *ec != '?') ++ec;
                if (ec < e -1) {
                    decoded.append(p, s-p);
                    decoded.append(decodeQuotedPrintable(s+10, ec-10-s));
                    s = p = ec + 2;
                } else {
                    s++;
                }
            } else {
                s++;
            }
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
MailInputStream::MailInputStream(StreamBase<char>* input)
        : SubStreamProvider(input), substream(0) {
    entrynumber = 0;
    nextLineStartPosition = 0;
    // parse the header and store the imporant header fields
    readHeader();
    if (status != Ok) {
        fprintf(stderr, "no valid header\n");
        return;
    }
}
MailInputStream::~MailInputStream() {
    if (substream && substream != entrystream) {
        delete substream;
    }
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
MailInputStream::readHeaderLine() {
    // state: 0 -> ok, 1 -> '\r', 2 -> '\n', 3 -> '\r\n'
    char state = 0;
    int32_t nread;
    int32_t linepos = 0;
    bool completeLine = false;
    char c = 0;

    input->reset(nextLineStartPosition);
    do {
        nread = input->read(linestart, linepos+1, maxlinesize);
        if (nread < linepos+1) {
            completeLine = true;
            lineend = linestart + nread;
            status = Eof;
            return;
        }
        input->reset(nextLineStartPosition);
        if (input->getStatus() == Error) {
            status = Error;
            error = input->getError();
            return;
        } else if (linepos >= maxlinesize) {
            // error line is too long
            status = Error;
            error = "mail header line is too long";
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
                    if (c == '\r' || !isspace(c)) { // end
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
MailInputStream::getValue(const char* n, const string& headerline) const {
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
    if (*v == '"') {
        value = string(v+1, vend-v-2);
    } else {
        value = string(v, vend-v);
    }
    return value;
}
void
MailInputStream::readHeader() {
    maxlinesize = 1000;

    readHeaderLine();
    while (status == Ok && linestart != lineend) {
        handleHeaderLine();
        readHeaderLine();
    }
}
/**
 * Read lines from the email until a line contains the boundary.
 * If a boundary is encountered, the block header is parsed.
 **/
void
MailInputStream::scanBody() {
    while (status == Ok) {
        readHeaderLine();
        string::size_type len = lineend - linestart;
        if (len > 2 && strncmp("--", linestart, 2) == 0) {
            string::size_type blen = boundary.top().length();
            if (len == blen + 4 && strncmp(linestart + 2 + blen, "--", 2) == 0
                    && strncmp(linestart + 2, boundary.top().c_str(), blen)
                        == 0) { 
                // check if this is the end of a multipart
                boundary.pop();
                if (boundary.size() == 0) {
                    status = Eof;
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
MailInputStream::handleHeaderLine() {
    static const char* subject = "Subject:";
    static const char* contenttype = "Content-Type:";
    static const char* contenttransferencoding = "Content-Transfer-Encoding:";
    static const char* contentdisposition = "Content-Disposition:";
    int32_t len = lineend - linestart;
    if (len < 2) return;
    if (len < 8) {
        return;
    } else if (strncasecmp(linestart, subject, 8) == 0) {
        int32_t offset = 8;
        while (offset < len && isspace(linestart[offset])) offset++;
        this->subject = getDecodedHeaderValue(linestart+offset, len-offset);
    } else if (strncasecmp(linestart, contenttype, 13) == 0) {
        int32_t offset = 13;
        while (offset < len && isspace(linestart[offset])) offset++;
        this->contenttype = std::string(linestart+offset, len-offset);
        // get the boundary
        string b = getValue("boundary", this->contenttype);
        if (b.size()) {
            boundary.push(b);
        }
    } else if (strncasecmp(linestart, contenttransferencoding, 26) == 0) {
        this->contenttransferencoding = std::string(linestart, len);
    } else if (strncasecmp(linestart, contentdisposition, 20) == 0) {
        this->contentdisposition = std::string(linestart, len);
    }
}
bool
MailInputStream::checkHeaderLine() const {
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
MailInputStream::handleBodyLine() {
    clearHeaders();

    // start of new block
    // read part header
    bool validheader;
    size_t n = boundary.size();
    do {
        readHeaderLine();
        validheader = status == Ok && checkHeaderLine();
        if (validheader) {
            handleHeaderLine();
        }
    } while (status == Ok && validheader);
    if (boundary.size() > n) {
        return false;
    }
    readHeaderLine();
    if (status != Ok) {
        return false;
    }

    // get the filename
    entryinfo.filename = getValue("filename", contentdisposition);
    if (entryinfo.filename.length() == 0) {
        entryinfo.filename = getValue("name", contenttype);
    }

    // create a stream that's limited to the content
    substream = new StringTerminatedSubStream(input, "--"+boundary.top());
    // set a reasonable buffer size
    if (strcasestr(contenttransferencoding.c_str(), "base64")) {
        entrystream = new Base64InputStream(substream);
    } else {
        entrystream = substream;
    }
    return true;
}
/**
 * Not all parts of multipart emails have a name. This function keeps a running
 * number to make sure all parts have a name so that they can be referenced
 * later.
 **/
void
MailInputStream::ensureFileName() {
    entrynumber++;
    if (entryinfo.filename.length() == 0) {
        ostringstream o;
        o << entrynumber;
        entryinfo.filename = o.str();
    }
    entryinfo.type = EntryInfo::File;
}
StreamBase<char>*
MailInputStream::nextEntry() {
    if (status != Ok) return 0;
    // if the mail does not consist of multiple parts, we give a pointer to
    // the input stream
    if (boundary.size() == 0) {
        // signal eof because we only return eof once
        status = Eof;
        entrystream = new SubInputStream(input);
        entryinfo.filename = "body";
        return entrystream;
    }
    // read anything that's left over in the previous stream
    if (substream) {
        const char* dummy;
        while (substream->getStatus() == Ok) {
            substream->read(dummy, 1, 0);
        }
        if (substream->getStatus() == Error) {
            status = Error;
        } else {
            nextLineStartPosition = substream->getOffset()
                + substream->getSize();
        }
        if (substream && substream != entrystream) {
            delete substream;
        }
        substream = 0;
        delete entrystream;
        entrystream = 0;

        if (status != Ok) {
            return 0;
        }
    }
    scanBody();

    if (entrystream == 0) {
        status = Eof;
    }
    ensureFileName();
    return entrystream;
}
void
MailInputStream::clearHeaders() {
    contenttype.resize(0);
    contenttransferencoding.resize(0);
    contentdisposition.resize(0);
}

