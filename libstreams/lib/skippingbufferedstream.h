/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006,2009 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_SKIPPINGBUFFEREDSTREAM_H
#define STRIGI_SKIPPINGBUFFEREDSTREAM_H

#include <strigi/streambase.h>
#include <strigi/streambuffer.h>
#include <cassert>
#include <iostream>

namespace Strigi {

/**
 * @brief Abstract class providing a buffered input stream.
 *
 * You can inherit this class to provide buffered access to a
 * resource.  You just need to implement fillBuffer and advanceStream, and
 * SkippingBufferedStream will do the rest.
 */
template <class T>
class SkippingBufferedStream : public StreamBase<T> {
private:
    StreamBuffer<T> buffer;
    bool finishedWritingToBuffer;

    void writeToBuffer(int32_t minsize, int32_t maxsize);
protected:
    /**
     * @brief Fill the buffer with the provided data
     *
     * This function should be implemented by subclasses.
     * It should write up to @p space characters from the
     * stream to the buffer position pointed to by @p start.
     *
     * If the end of the stream is encountered, -1 should be
     * returned.
     *
     * If an error occurs, the status should be set to Error,
     * an error message should be set and -1 should be returned.
     *
     * You should @em not call this function yourself.
     *
     * @param start where the data should be written to
     * @param space the maximum amount of data to write
     * @return Number of characters written, or -1 on error
     **/
    virtual int32_t fillBuffer(T* start, int32_t space) = 0;
    virtual int64_t advanceInput(int64_t space);
    /**
     * @brief Resets the buffer, allowing it to be used again
     *
     * This function resets the buffer, allowing it to be re-used.
     */
    void resetBuffer() {
        StreamBase<T>::m_size = -1;
        StreamBase<T>::m_position = 0;
        StreamBase<T>::m_error.assign("");
        StreamBase<T>::m_status = Ok;
        buffer.readPos = buffer.start;
        buffer.avail = 0; 
        finishedWritingToBuffer = false;
    }
    /**
     * @brief Sets the minimum size of the buffer
     */
    void setMinBufSize(int32_t s) {
        buffer.makeSpace(s);
    }
    SkippingBufferedStream<T>();
public:
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t reset(int64_t pos);
    virtual int64_t skip(int64_t ntoskip);
};


/** Abstract class for a buffered stream of bytes */
typedef SkippingBufferedStream<char> SkippingBufferedInputStream;

/** Abstract class for a buffered stream of Unicode characters */
typedef SkippingBufferedStream<wchar_t> SkippingBufferedReader;


template <class T>
SkippingBufferedStream<T>::SkippingBufferedStream() {
    finishedWritingToBuffer = false;
}

template <class T>
void
SkippingBufferedStream<T>::writeToBuffer(int32_t ntoread, int32_t maxread) {
    int32_t missing = ntoread - buffer.avail;
    int32_t nwritten = 0;
    while (missing > 0 && nwritten >= 0) {
        int32_t space;
        space = buffer.makeSpace(missing);
        if (maxread >= ntoread && space > maxread) {
             space = maxread;
        }
        T* start = buffer.readPos + buffer.avail;
        nwritten = fillBuffer(start, space);
        assert(StreamBase<T>::m_status != Eof);
        if (nwritten > 0) {
            buffer.avail += nwritten;
            missing = ntoread - buffer.avail;
        }
    }
    if (nwritten < 0) {
        finishedWritingToBuffer = true;
    }
}
template <class T>
int32_t
SkippingBufferedStream<T>::read(const T*& start, int32_t min, int32_t max) {
    if (StreamBase<T>::m_status == Error) return -2;
    if (StreamBase<T>::m_status == Eof) return -1;

    // do we need to read data into the buffer?
    if (min > max) max = 0;
    if (!finishedWritingToBuffer && min > buffer.avail) {
        // do we have enough space in the buffer?
        writeToBuffer(min, max);
        if (StreamBase<T>::m_status == Error) return -2;
    }

    int32_t nread = buffer.read(start, max);

    StreamBase<T>::m_position += nread;
    if (StreamBase<T>::m_position > StreamBase<T>::m_size
        && StreamBase<T>::m_size > 0) {
        // error: we read more than was specified in size
        // this is an error because all dependent code might have been labouring
        // under a misapprehension
        StreamBase<T>::m_status = Error;
        StreamBase<T>::m_error = "Stream is longer than specified.";
        nread = -2;
    } else if (StreamBase<T>::m_status == Ok && buffer.avail == 0
            && finishedWritingToBuffer) {
        StreamBase<T>::m_status = Eof;
        if (StreamBase<T>::m_size == -1) {
            StreamBase<T>::m_size = StreamBase<T>::m_position;
        }
        // save one call to read() by already returning -1 if no data is there
        if (nread == 0) nread = -1;
    }
    return nread;
}
template <class T>
int64_t
SkippingBufferedStream<T>::reset(int64_t newpos) {
    assert(newpos >= 0);
    if (StreamBase<T>::m_status == Error) return -2;
    // check to see if we have this position
    int64_t d = StreamBase<T>::m_position - newpos;
    if (buffer.readPos - d >= buffer.start && -d < buffer.avail) {
        StreamBase<T>::m_position -= d;
        buffer.avail += (int32_t)d;
        buffer.readPos -= d;
        StreamBase<T>::m_status = Ok;
    }
    return StreamBase<T>::m_position;
}
template <class T>
int64_t
SkippingBufferedStream<T>::skip(int64_t ntoskip) {
    assert(ntoskip >= 0);
    if (ntoskip == 0) return 0;
    if (buffer.avail > ntoskip) {
        buffer.avail -= (int32_t)ntoskip;
        buffer.readPos += ntoskip;
        StreamBase<T>::m_position += ntoskip;
        return ntoskip;
    }
    int32_t frombuffer = buffer.avail;
    StreamBase<T>::m_position += frombuffer;
    int64_t oldpos = StreamBase<T>::m_position;
    buffer.avail = 0;
    ntoskip -= frombuffer;
    int64_t n = (ntoskip) ?advanceInput(ntoskip) :0;
    if (n < 0) {
        StreamBase<T>::m_status = Error;
        return -2;
    }
    StreamBase<T>::m_position = oldpos + n;
    if (n < ntoskip) {
        StreamBase<T>::m_status = Eof;
        if (StreamBase<T>::m_size == -1) {
            StreamBase<T>::m_size = StreamBase<T>::m_position;
        }
    }
    return n + frombuffer;
}
template <class T>
int64_t
SkippingBufferedStream<T>::advanceInput(int64_t ntoskip) {
    const T *begin;
    int32_t nread;
    int64_t skipped = 0;
    while (ntoskip) {
        int32_t step = (int32_t)((ntoskip > buffer.size) ?buffer.size :ntoskip);
        nread = read(begin, 1, step);
        if (nread <= 0) {
            return skipped;
        }
        ntoskip -= nread;
        skipped += nread;
    }
    return skipped;
}

} // end namespace Strigi

#endif
