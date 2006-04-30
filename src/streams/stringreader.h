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
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t mark(int32_t readlimit);
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
StringReader<T>::read(const T*& start, int32_t min, int32_t max) {
    int64_t left = StreamBase<T>::size - StreamBase<T>::position;
    if (left == 0) {
        StreamBase<T>::status = Eof;
        return 0;
    }
    if (min < 0) min = 0;
    int32_t nread = (int32_t)(max > left || max <= min) ?left :max;
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
    return read(start, ntoskip, ntoskip);
}
template <class T>
int64_t
StringReader<T>::mark(int32_t /*readlimit*/) {
    markpt = StreamBase<T>::position;
    return markpt;
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
