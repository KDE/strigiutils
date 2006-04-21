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
};

template <class T>
BufferedInputStream<T>::BufferedInputStream<T>() {
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

    return buffer.read(start);
}
template <class T>
int32_t
BufferedInputStream<T>::read(const T*& start, int32_t ntoread) {
    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    writeToBuffer(ntoread);

    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    return buffer.read(start, ntoread);
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
//    printf("reset %p %p\n", this, buffer.markPos);
    if (buffer.markPos) {
        buffer.reset();
        return Ok;
    } else {
        StreamBase<T>::error = "No valid mark for reset.";
        return Error;
    }
}
}

#endif
