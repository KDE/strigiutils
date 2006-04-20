#include "inputstreambuffer.h"

template <class T>
InputStreamBuffer<T>::InputStreamBuffer() {
    markPos = readPos = start = NULL;
    size = avail = 0;
}
template <class T>
InputStreamBuffer<T>::~InputStreamBuffer() {
    free(start);
}
template <class T>
void
InputStreamBuffer<T>::setSize(int32_t size) {
    // store pointer information
    int32_t offset = readPos - start;
    int32_t markOffset = (markPos) ? markPos - start : -1;

    // allocate memory in the buffer
    start = (T*)realloc(start, size*sizeof(T));
    this->size = size;

    // restore pointer information
    readPos = start + offset;
    markPos = (markOffset == -1) ?0 :start + markOffset;
}
template <class T>
void
InputStreamBuffer<T>::mark(int32_t limit) {
    // if we had a larger limit defined for the same position, do nothing
    if (readPos == markPos && limit <= markLimit) {
        return;
    }
            
    markLimit = limit;
    // if we have enough room, only set the mark
    int32_t offset = readPos - start;
    if (size - offset >= limit) {
        markPos = readPos;
        return;
    }

    // if we don't have enough room start by
    // moving memory to the start of the buffer
    if (readPos != start) {
        memmove(start, readPos, avail*sizeof(T));
        readPos = start;
    }

    // if we have enough room now, finish
    if (size >= limit) {
        markPos = readPos;
        return;
    }

    // last resort: increase buffer size
    setSize(limit);
    markPos = readPos;
}
template <class T>
void
InputStreamBuffer<T>::reset() {
    if (markPos != 0) {
        avail += readPos - markPos;
        readPos = markPos;
    }
}
template <class T>
int32_t
InputStreamBuffer<T>::makeSpace(int32_t needed) {
    int32_t space = size - (readPos - start) - avail;
    if (space < needed) { // buffer is too small
            // may we move the data in the buffer?
        bool maymove = (markPos == 0 || readPos - markPos > markLimit);
        if (maymove) {
        }
    }

    if (markPos == 0 && avail == 0) {
        // if there is no mark
        // we set the next write to the start of the buffer
        markPos = 0;
        writeSpace = size;
        readPos = start;
    }
    return writeSpace;
}
template <class T>
int32_t
InputStreamBuffer<T>::read(const T*& start, int32_t max) {
    start = readPos;
    if (max <= 0 || max > avail) {
        max = avail;
    }
    readPos += max;
    avail -= max;
    return max;
}
