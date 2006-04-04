#include "inputstreambuffer.h"

template <class T>
InputStreamBuffer<T>::InputStreamBuffer() {
    markPos = curPos = start = NULL;
    size = avail = 0;
}
template <class T>
InputStreamBuffer<T>::~InputStreamBuffer() {
    free(start);
}
template <class T>
void
InputStreamBuffer<T>::setSize(size_t size) {
    // store pointer information
    size_t offset = curPos - start;
    // allocate memory in the buffer
    start = (T*)realloc(start, size*sizeof(T));
    this->size = size;
    // restore pointer information
    curPos = start + offset;
}
template <class T>
void
InputStreamBuffer<T>::mark(size_t readlimit) {
    // if we have enough room, dont change anything
    size_t offset = curPos - start;
    if (size - offset >= readlimit) {
        markPos = curPos;
        return;
    }

    // if we don't have enough room start by
    // moving memory to the start of the buffer
    if (curPos != start) {
        memmove(start, curPos, avail*sizeof(T));
        curPos = start;
    }

    // if we have enough room now, finish
    if (size >= readlimit) {
        markPos = curPos;
        return;
    }

    // last resort: increase buffer size
    setSize(readlimit);
    markPos = curPos;
}
template <class T>
void
InputStreamBuffer<T>::reset() {
    if (markPos != NULL) {
        avail += curPos - markPos;
        curPos = markPos;
    }
}
template <class T>
size_t
InputStreamBuffer<T>::getWriteSpace() {
    // advance the buffer to where the last write left off
    curPos += avail;
    // calculate how much space is left at the end of the buffer
    size_t writeSpace = size - (curPos - start);
    if (markPos == NULL || writeSpace <= 0) {
        // if there is no mark or the buffer is full,
        // we set the next write to the start of the buffer
        markPos = NULL;
        writeSpace = size;
        curPos = start;
    }
    return writeSpace;
}
template <class T>
void
InputStreamBuffer<T>::read(const T*& start, size_t& nread, size_t max) {
    start = curPos;
    if (max <= 0 || max > avail) {
        max = avail;
    }
    curPos += max;
    avail -= max;
    nread = max;
}
