#ifndef INPUTSTREAMBUFFER_H
#define INPUTSTREAMBUFFER_H

#include <cstdlib>

namespace jstreams {

template <class T>
class InputStreamBuffer {
private:
public:
    T* start;
    int32_t size;
    T* readPos;
    int32_t avail;
    T* markPos;
    int32_t markLimit;

    InputStreamBuffer();
    ~InputStreamBuffer();
    void setSize(int32_t size);
    void mark(int32_t readlimit);
    void reset();
    int32_t read(const T*& start, int32_t max=0);

    /**
     * This function prepares the buffer for a new write.
     * returns the number of available places.
     **/
     int32_t makeSpace(int32_t needed);
};

} // end namespace jstreams

#endif
