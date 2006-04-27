#ifndef STRINGREADER_H
#define STRINGREADER_H

#include "streambase.h"

namespace jstreams {

template <class T>
class StringReader : public StreamBase<T> {
private:
    int64_t markpt;
    T* data;
    bool dataowner;
    StringReader(const StringReader<T>&);
    void operator=(const StringReader<T>&);
public:
    StringReader(const T* value, int32_t length = -1, bool copy = true);
    ~StringReader();
    int32_t read(const T*& start);
    int32_t read(const T*& start, int32_t ntoread);
    int32_t readAtLeast(const T*& start, int32_t ntoread);
    int64_t skip(int64_t ntoskip);
    StreamStatus mark(int32_t readlimit);
    StreamStatus reset();
};

template <class T>
StringReader<T>::StringReader(const T* value, int32_t length, bool copy)
        : markpt(0), dataowner(copy) {
    if (length < 0) {
        if (sizeof(T) > 1) {
            length = wcslen((const wchar_t*)value);
        } else {
            length = strlen((const char*)value);
        }
    }
    StreamBase<T>::size = length;
    if (copy) {
        data = new T[length+1];
        size_t s = (size_t)(length*sizeof(T));
        memcpy(data, value, s);
        data[length] = 0;
    } else {
        // casting away const is ok, because we don't write anyway
        data = (T*)value;
    }
}
template <class T>
StringReader<T>::~StringReader() {
    if (dataowner) {
        delete [] data;
    }
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
int32_t
StringReader<T>::readAtLeast(const T*& start, int32_t ntoread) {
    return read(start);
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
