#ifndef INPUTSTREAMBUFFER_H
#define INPUTSTREAMBUFFER_H

#include <cstdio>

template <class T>
class InputStreamBuffer {
private:
public:
    T *start;
    size_t size;
    T *curPos;
    size_t avail;
    T *markPos;

    InputStreamBuffer();
    ~InputStreamBuffer();
    void setSize(size_t size);
    void mark(size_t readlimit);
    void reset();
    void read(const T*& start, size_t& end, size_t max);

    /**
     * This function prepares the buffer for a new write.
     * Any data not read since the last write is lost.
     * returns the number of available places.
     **/
    size_t getWriteSpace();
};

#endif
