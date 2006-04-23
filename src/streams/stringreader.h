#ifndef STRINGREADER_H
#define STRINGREADER_H

#include "streambase.h"

namespace jstreams {

template <class T>
class StringReader : public StreamBase<T> {
private:
    T* data;
    int64_t markpt;
    StringReader(const StringReader<T>&);
    void operator=(const StringReader<T>&);
public:
    StringReader(const T* value, const int32_t length);
    ~StringReader();
    int32_t read(const T*& start);
    int32_t read(const T*& start, int32_t ntoread);
    int64_t skip(int64_t ntoskip);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

template <class T>
StringReader<T>::StringReader(const T* value, const int32_t length )
        : markpt(0) {
    StreamBase<T>::size = length;
    data = new T[length+1];
    size_t s = (size_t)(length*sizeof(T));
    memcpy(data, value, s);
    data[length] = 0;
}
template <class T>
StringReader<T>::~StringReader() {
    delete [] data;
}
template <class T>
int32_t
StringReader<T>::read(const T*& start) {
    StreamBase<T>::status = Eof;
    int64_t left = StreamBase<T>::size - StreamBase<T>::position;
    if (left == 0) {
        return 0;
    }
    int32_t nread = (int32_t)((left > INT32MAX) ?INT32MAX :left);
    start = data + StreamBase<T>::position;
    StreamBase<T>::position = StreamBase<T>::size;
    return nread;
}
template <class T>
int32_t
StringReader<T>::read(const T*& start, int32_t ntoread) {
    int64_t left = StreamBase<T>::size - StreamBase<T>::position;
    if (left == 0) {
        StreamBase<T>::status = Eof;
        return 0;
    }
    int32_t nread = (int32_t)(ntoread > left) ?left :ntoread;
    start = data + StreamBase<T>::position;
    StreamBase<T>::position += nread;
    if (StreamBase<T>::position == StreamBase<T>::size) {
        StreamBase<T>::status = Eof;
    }
    return nread;
}
template <class T>
int64_t
StringReader<T>::skip(int64_t ntoskip) {
    const T* start;
    return read(start, ntoskip);
}
template <class T>
StreamStatus
StringReader<T>::mark(int32_t /*readlimit*/) {
    markpt = StreamBase<T>::position;
    return Ok;
}
template <class T>
StreamStatus
StringReader<T>::reset() {
    StreamBase<T>::position = markpt;
    if (markpt == StreamBase<T>::size) {
        StreamBase<T>::status = Eof;
    } else {
        StreamBase<T>::status = Ok;
    }
    return Ok;
}

} // end namespace jstreams

#endif
