#include "inputstreambuffer.h"

template <class T>
InputStreamBuffer<T>::InputStreamBuffer() {
    markPos = readPos = start = 0;
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
    // if there's no buffer yet, allocate one now
    if (start == 0) {
        setSize(limit+1);
    }
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
    setSize(limit+1);
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
    // determine how much space is available for writing
    int32_t space = size - (readPos - start) - avail;
    if (space >= needed) {
        // there's enough space
        return space;
    }

    if (markPos && readPos - markPos <= markLimit) {
        // move data to the start of the buffer while respecting the set mark
        if (markPos != start) {
            printf("moving with mark\n");
            int32_t n = avail + readPos - markPos;
            memmove(start, markPos, n*sizeof(T));
            readPos -= markPos - start;
            space += markPos - start;
            markPos = start;
        }
    } else if (avail) {
        if (readPos != start) {
            printf("moving\n");
            // move data to the start of the buffer
            memmove(start, readPos, avail*sizeof(T));
            space += readPos - start;
            readPos = start;
            markPos = 0;
        }
    } else {
        // we may start writing at the start of the buffer
        markPos = 0;
        readPos = start;
        space = size;
    }
    if (space >= needed) {
        // there's enough space now
        return space;
    }

    // still not enough space, we have to allocate more
    printf("resize %i %i %i %i %i\n", avail, needed, space, size + needed - space, size);
    setSize(size + needed - space);
    return needed;
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
