#ifndef INPUTSTREAMBUFFER_H
#define INPUTSTREAMBUFFER_H

#include <cstdlib>

template <class T>
class InputStreamBuffer {
private:
public:
    T *start;
    int32_t size;
    T *curPos;
    int32_t avail;
    T *markPos;

    InputStreamBuffer();
    ~InputStreamBuffer();
    void setSize(int32_t size);
    void mark(int32_t readlimit);
    void reset();
    void read(const T*& start, int32_t& end, int32_t max);

    /**
     * This function prepares the buffer for a new write.
     * Any data not read since the last write is lost.
     * returns the number of available places.
     **/
    int32_t getWriteSpace();
};

#endif
