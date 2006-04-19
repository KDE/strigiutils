#ifndef BUFFEREDSTREAM_H
#define BUFFEREDSTREAM_H

#include "streambase.h"
#include "inputstreambuffer.h"

namespace jstreams {

template <class T>
class BufferedInputStream : public StreamBase<T> {
protected:
    InputStreamBuffer<T> buffer;
    virtual void fillBuffer() = 0;
public:
    int32_t read(const T*& start);
    int32_t read(const T*& start, int32_t ntoread);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

template <class T>
int32_t
BufferedInputStream<T>::read(const T*& start) {
    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    if (buffer.avail < 1) {
        do {
            fillBuffer();
        } while (StreamBase<T>::status == Ok && buffer.avail < 1);
        if (StreamBase<T>::status == Error) return -1;
        if (StreamBase<T>::status == Eof) return 0;
    }

    return buffer.read(start);
}
template <class T>
int32_t
BufferedInputStream<T>::read(const T*& start, int32_t ntoread) {
    if (StreamBase<T>::status == Error) return -1;
    if (StreamBase<T>::status == Eof) return 0;

    if (buffer.avail < ntoread) {
        // make sure the buffer is large enough
        buffer.setSize(ntoread);
        do {
            fillBuffer();
        } while (StreamBase<T>::status == Ok && buffer.avail < ntoread);
        if (StreamBase<T>::status == Error) return -1;
        if (StreamBase<T>::status == Eof) return 0;
    }

    return buffer.read(start, ntoread);
}
template <class T>
StreamStatus
BufferedInputStream<T>::mark(int32_t readlimit) {
    buffer.mark(readlimit);
    return Ok;
}
template <class T>
StreamStatus
BufferedInputStream<T>::reset() {
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
