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
    int32_t read_(const T*& start, int32_t min, int32_t max);
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
    // this function might be useful if you want to reuse a bufferedstream
    void resetBuffer() {printf("implement 'resetBuffer'\n");}
public:
    BufferedInputStream<T>();
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t);
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
BufferedInputStream<T>::read(const T*& start, int32_t min, int32_t max) {
    if (StreamBase<T>::status == Error) return -2;
    if (StreamBase<T>::status == Eof) return -1;

    writeToBuffer(min);

    if (StreamBase<T>::status == Error) return -2;
    if (StreamBase<T>::status == Eof) return -1;

    int32_t nread = buffer.read(start, max);
    BufferedInputStream<T>::position += nread;
    if (BufferedInputStream<T>::status == Ok && buffer.avail == 0
            && finishedWritingToBuffer) {
        BufferedInputStream<T>::status = Eof;
        // save one call to read() by already returning -1 if no data is there
        if (nread == 0) nread = -1;
    }
    return nread;
}
template <class T>
int64_t
BufferedInputStream<T>::mark(int32_t readlimit) {
    buffer.mark(readlimit);
//    printf("mark %p %i %i\n", this, readlimit, buffer.size);
    return StreamBase<T>::position;
}
template <class T>
int64_t
BufferedInputStream<T>::reset(int64_t newpos) {
    if (StreamBase<T>::status == Error) return -2;
    // check to see if we have this position
    int64_t d = BufferedInputStream<T>::position - newpos;
    if (buffer.readPos - d >= buffer.start && -d < buffer.avail) {
        BufferedInputStream<T>::position -= d;
        buffer.avail += d;
        buffer.readPos -= d;
        StreamBase<T>::status = Ok;
    }
    return StreamBase<T>::position;
}
template <class T>
int64_t
BufferedInputStream<T>::skip(int64_t ntoskip) {
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
}

#endif
