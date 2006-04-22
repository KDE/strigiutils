#ifndef BUFFEREDSTREAM_H
#define BUFFEREDSTREAM_H

#include "streambase.h"
#include "inputstreambuffer.h"

namespace jstreams {

template <class T>
class BufferedInputStream : public StreamBase<T> {
private:
    bool finishedWritingToBuffer;
    InputStreamBuffer<T> buffer;

    void writeToBuffer(int32_t minsize);
protected:
    /**
     * This function must be implemented by the subclasses.
     * It should write a maximum of @p space characters at the buffer
     * position pointed to by @p start. If no more data is avaiable due to
     * end of file, -1 should be returned. If an error occurs, the status
     * should be set to Error, an error message should be set and the function
     * must return -1.
     **/
    virtual int32_t fillBuffer(T* start, int32_t space) = 0;
    void resetBuffer() {printf("implement 'resetBuffer'\n");}
public:
    BufferedInputStream<T>();
    int32_t read(const T*& start);
    int32_t read(const T*& start, int32_t ntoread);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
    virtual int64_t skip(int64_t ntoskip);
};

template <class T>
BufferedInputStream<T>::BufferedInputStream() {
    finishedWritingToBuffer = false;
}

template <class T>
void
BufferedInputStream<T>::writeToBuffer(int32_t ntoread) {
    int32_t missing = ntoread - buffer.avail;
    if (missing > 0 && finishedWritingToBuffer) {
        StreamBase<T>::status = Eof;
        return;
    }
    int32_t nwritten = 0;
    while (missing > 0 && nwritten >= 0) {
        int32_t space;
        space = buffer.makeSpace(missing);
        T* start = buffer.readPos + buffer.avail;
        nwritten = fillBuffer(start, space);
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
BufferedInputStream<T>::read(const T*& start) {
    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    writeToBuffer(1);

    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    int32_t nread = buffer.read(start);
    BufferedInputStream<T>::position += nread;
    if (BufferedInputStream<T>::status == Ok && buffer.avail == 0
            && finishedWritingToBuffer) {
        BufferedInputStream<T>::status = Eof;
    }
    return nread;
}
template <class T>
int32_t
BufferedInputStream<T>::read(const T*& start, int32_t ntoread) {
    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    writeToBuffer(ntoread);

    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    int32_t nread = buffer.read(start, ntoread);
    BufferedInputStream<T>::position += nread;
    if (BufferedInputStream<T>::status == Ok && buffer.avail == 0
            && finishedWritingToBuffer) {
        BufferedInputStream<T>::status = Eof;
    }
    return nread;
}
template <class T>
StreamStatus
BufferedInputStream<T>::mark(int32_t readlimit) {
//    printf("mark %p %i\n", this, readlimit);
    buffer.mark(readlimit);
    return Ok;
}
template <class T>
StreamStatus
BufferedInputStream<T>::reset() {
    if (buffer.markPos) {
        BufferedInputStream<T>::position -= buffer.readPos - buffer.markPos;
        buffer.reset();
        return Ok;
    } else {
        StreamBase<T>::error = "No valid mark for reset.";
        return Error;
    }
}
template <class T>
int64_t
BufferedInputStream<T>::skip(int64_t ntoskip) {
    const T *begin;
    int32_t nread;
    int64_t skipped = 0;
    while (ntoskip) {
        int32_t step = (int32_t)((ntoskip > buffer.size) ?buffer.size :ntoskip);
        nread = read(begin, step);
        if (nread <= 0) {
            return skipped;
        }
        ntoskip -= nread;
        skipped += nread;
    }
    return skipped;
}
}

#endif
