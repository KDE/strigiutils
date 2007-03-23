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
#include "lineeventanalyzer.h"
#include "streamlineanalyzer.h"
#include "analysisresult.h"
#include "textutils.h"
#include <cstring>
#include <cassert>
using namespace Strigi;
using namespace jstreams;
using namespace std;

// end of line is \r, \n or \r\n

LineEventAnalyzer::LineEventAnalyzer(vector<StreamLineAnalyzer*>& l)
        :line(l), ready(true), initialized(false) {
}
LineEventAnalyzer::~LineEventAnalyzer() {
    vector<StreamLineAnalyzer*>::iterator l;
    for (l = line.begin(); l != line.end(); ++l) {
        delete *l;
    }
}
void
LineEventAnalyzer::startAnalysis(AnalysisResult* r) {
    result = r;
    ready = line.size() == 0;
    initialized = false;
    sawCarriageReturn = false;
    missingBytes = 0;
    lineBuffer.assign("");
    byteBuffer.assign("");
}
void
LineEventAnalyzer::endAnalysis() {
    vector<StreamLineAnalyzer*>::iterator l;
    for (l = line.begin(); l != line.end(); ++l) {
        (*l)->endAnalysis();
    }
}
void
LineEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (ready) return;
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
            missingBytes -= length;
            return;
        }
    }

    // validate the utf8
    const char* p = checkUtf8(data, length, missingBytes);
    if (p) {
        // the data ends in an incomplete character
        if (missingBytes > 0) {
            int32_t charStartSize = length - (p - data);
            // store the start of the character
            byteBuffer.assign(p, charStartSize);
            // do not consider this incomplete character in the rest of this
            // function
            length -= charStartSize;
        } else {
            // not valid
            ready = true;
            return;
        }
    }

    // find the first \n
    p = data;
    const char* end = data + length;
    do {
        if (*p == '\n' || *p == '\r') break; 
    } while (++p != end);
    if (p == end) { // no '\n' was found, we put this in the buffer
        lineBuffer.append(data, length);
        return;
    }
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
        lineBuffer.append(data, p-data);
        emit(lineBuffer.c_str(), lineBuffer.size());
        lineBuffer.assign("");
    } else {
        emit(data, p-data);
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
        emit(data, p-data);
        if (ready) return;
    }
}
void
LineEventAnalyzer::emit(const char*data, uint32_t length) {
//    fprintf(stderr, "%.*s\n", length, data);
    bool more = false;
    vector<StreamLineAnalyzer*>::iterator i;
    if (!initialized) {
        for (i = line.begin(); i != line.end(); ++i) {
            (*i)->startAnalysis(result);
            more = more || !(*i)->isReadyWithStream();
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