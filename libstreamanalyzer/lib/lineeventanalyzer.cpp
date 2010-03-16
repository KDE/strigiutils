/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

#include "lineeventanalyzer.h"
#include <strigi/streamlineanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/textutils.h>
#include <cstring>
#include <cassert>
#include <cerrno>
using namespace Strigi;
using namespace std;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

// end of line is \r, \n or \r\n
#define CONVBUFSIZE 65536

LineEventAnalyzer::LineEventAnalyzer(vector<StreamLineAnalyzer*>& l)
        :line(l), converter((iconv_t)-1), numAnalyzers((uint)l.size()),
         convBuffer(new char[CONVBUFSIZE]), ready(true), initialized(false) {
    started = new bool[l.size()];
    for (uint i=0; i<numAnalyzers; ++i) {
        started[i] = false;
    }
}
LineEventAnalyzer::~LineEventAnalyzer() {
    vector<StreamLineAnalyzer*>::iterator l;
    for (l = line.begin(); l != line.end(); ++l) {
        delete *l;
    }
    if (converter != (iconv_t)-1) {
        iconv_close(converter);
    }
    delete [] convBuffer;
    delete [] started;
}
void
LineEventAnalyzer::startAnalysis(AnalysisResult* r) {
    result = r;
    ready = numAnalyzers == 0;
    initialized = false;
    sawCarriageReturn = false;
    missingBytes = 0;
    iMissingBytes = 0;
    lineBuffer.assign("");
    byteBuffer.assign("");
    ibyteBuffer.assign("");
    initEncoding(r->encoding());
    for (uint i=0; i < numAnalyzers; ++i) {
        started[i] = false;
    }
}
void
LineEventAnalyzer::initEncoding(std::string enc) {
    if (enc.size() == 0 || enc == "UTF-8") {
        encoding.assign("UTF-8");
        if (converter != (iconv_t)-1) {
            iconv_close(converter);
            converter = (iconv_t)-1;
        }
    } else if (converter != (iconv_t)-1 && encoding == enc) {
        // reset the converter
        iconv(converter, 0, 0, 0, 0);
    } else {
        encoding = enc;
        if (converter != (iconv_t)-1) {
            iconv_close(converter);
        }
        converter = iconv_open(encoding.c_str(), "UTF-8");
    }
}
void
LineEventAnalyzer::endAnalysis(bool complete) {
    // flush the last line if it did not end with a newline character
    if(complete && lineBuffer.size() > 0) {
        emitData(lineBuffer.c_str(), (uint32_t)lineBuffer.size());
        lineBuffer.assign("");
    }

    for (uint i=0; i < numAnalyzers; ++i) {
        if (started[i]) {
            line[i]->endAnalysis(complete);
        }
    }
}
void
LineEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (ready) return;
    if (converter == (iconv_t)-1) {
        handleUtf8Data(data, length);
        return;
    }
    size_t r;
    ICONV_CONST char *inbuf;
    char* outbuf;
    size_t inbytesleft;
    size_t outbytesleft;
    if (iMissingBytes) {
        if (iMissingBytes > length) {
            ibyteBuffer.append(data, length);
            iMissingBytes = (unsigned char)(iMissingBytes - length);
            return;
        } else {
            ibyteBuffer.append(data, iMissingBytes);
            data += iMissingBytes;
            length -= iMissingBytes;
            inbuf = (char*)ibyteBuffer.c_str();
            inbytesleft = ibyteBuffer.length();
            outbytesleft = CONVBUFSIZE;
            outbuf = convBuffer;
            r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
            if (r == (size_t)-1) { // must be an error
                ready = true;
                return;
            }
            handleUtf8Data(convBuffer, (uint32_t)(CONVBUFSIZE-outbytesleft));
        }
    }
    do {
        inbuf = (char*)data;
        inbytesleft = length;
        outbuf = convBuffer;
        outbytesleft = CONVBUFSIZE;
        r = iconv(converter, &inbuf, &inbytesleft, &outbuf,
            &outbytesleft);
        int32_t left = (uint32_t)(CONVBUFSIZE-outbytesleft);
        if (r == (size_t)-1) {
            uint32_t read;
            switch (errno) {
            case EINVAL: // last character is incomplete
                handleUtf8Data(convBuffer, left);
                ibyteBuffer.assign(inbuf, inbytesleft);
                iMissingBytes = (unsigned char)(length - (inbuf-data));
                return;
            case E2BIG: // output buffer is full
                handleUtf8Data(convBuffer, left);
                read = (uint32_t)(inbuf-data);
                data += read;
                length -= read;
                break;
            case EILSEQ: //invalid multibyte sequence
            default:
                ready = true;
                return;
            }
        } else { //input sequence was completely converted
            handleUtf8Data(convBuffer, left);
            return;
        }
    } while (true);
}
void
LineEventAnalyzer::handleUtf8Data(const char* data, uint32_t length) {
    assert(!(sawCarriageReturn && missingBytes > 0));

    // if the last block ended with '\r', the next '\n' can be skipped
    if (sawCarriageReturn) {
        if (length > 0 && data[0] == '\n') {
            data++;
            length--;
        }
        sawCarriageReturn = false;
    }

    // if we have incomplete characters left over from the last call,
    // complete them and validate them
    if (missingBytes > 0) {
        if (length > (unsigned char)missingBytes) {
            // we have enough data to finish the character
            byteBuffer.append(data, missingBytes);
            if (!checkUtf8(byteBuffer)) {
                // invalid utf8, nothing more to see here
                ready = true;
                return;
            }
            lineBuffer.append(byteBuffer);
            data += missingBytes;
            length -= missingBytes;
            // clean up the byte buffer
            byteBuffer.assign("");
            missingBytes = 0;
        } else {
            // not enough data, store it and wait for the next round
            byteBuffer.append(data, length);
            missingBytes = (unsigned char)(missingBytes - length);
            return;
        }
    }

    // validate the utf8
    const char* p = checkUtf8(data, length, missingBytes);
    if (p) {
        // the data ends in an incomplete character
        if (missingBytes > 0) {
            string::size_type charStartSize = length - (p - data);
            // store the start of the character
            byteBuffer.assign(p, charStartSize);
            // do not consider this incomplete character in the rest of this
            // function
            length = (uint32_t)(length - charStartSize);
        } else {
            // not valid
            ready = true;
            return;
        }
    }

    // find the first \n
    p = data;
    const char* end = data + length;
    while (p < end) {
        if (*p == '\n' || *p == '\r') break;
        p++; 
    }
    if (p == end) { // no '\n' was found, we put this in the buffer
        lineBuffer.append(data, length);
        return;
    }
    const char* lineend = p;
    if (*p == '\r') {
        // if \r is followed by \n, we can ignore \n
        if (p + 1 != end) {
            if (p[1] == '\n') {
                p++;
            }
        } else {
            sawCarriageReturn = true;
        }
    }

    // handle the first line from this call
    if (lineBuffer.size()) {
        lineBuffer.append(data, lineend-data);
        emitData(lineBuffer.c_str(), (uint32_t)lineBuffer.size());
        lineBuffer.assign("");
    } else {
        emitData(data, (uint32_t)(p-data));
    }
    if (ready) return;

    // handle the other lines
    while (++p != end) {
        data = p;
        do {
            if (*p == '\n' || *p == '\r') break; 
        } while (++p != end);
        if (p == end) {
            lineBuffer.assign(data, end-data);
            break;
        }
        lineend = p;
        if (*p == '\r') {
            // if \r is followed by \n, we can ignore \n
            if (p + 1 != end) {
                if (p[1] == '\n') {
                    p++;
                }
            } else {
                sawCarriageReturn = true;
            }
        }
        emitData(data, (uint32_t)(lineend-data));
        if (ready) return;
    }
}
void
LineEventAnalyzer::emitData(const char*data, uint32_t length) {
//    fprintf(stderr, "%.*s\n", length, data);
    bool more = false;
    vector<StreamLineAnalyzer*>::iterator i;
    if (!initialized) {
        for (uint j = 0; j < numAnalyzers; ++j) {
            StreamLineAnalyzer* s = line[j];
            s->startAnalysis(result);
            started[j] = true;
            more = more || !s->isReadyWithStream();
        }
        initialized = true;
        ready = !more;
        if (ready) {
            return;
        }
        more = false;
    }
    for (i = line.begin(); i != line.end(); ++i) {
        if (!(*i)->isReadyWithStream()) {
            (*i)->handleLine(data, length);
        }
        more = more || !(*i)->isReadyWithStream();
    }
    ready = !more;
}
bool
LineEventAnalyzer::isReadyWithStream() {
    return ready;
}
