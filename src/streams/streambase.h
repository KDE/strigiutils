#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <string>

#define INT32MAX 0x7FFFFFFFL

namespace jstreams {

enum StreamStatus { Ok, Eof, Error };

/**
 * @short Base class for stream read access to many different file types.
 * 
 * This class is based on the interface java.io.InputStream. It allows
 * for uniform access to streamed resources.
 * The main difference with the java equivalent is a performance improvement.
 * When reading data, data is not copied into a buffer provided by the caller,
 * but a pointer to the read data is provided. This makes this interface especially
 * useful for deriving from it and implementing filterers or transformers.
 */
// java mapping: long=int64, int=int32, byte=uint8_t
template <class T>
class StreamBase {
protected:
    StreamStatus status;
    std::string error;
public:
    virtual ~StreamBase(){}
    /**
     * Return a string representation of the last error that has occurred.
     * If no error has occurred, an empty string is returned.
     **/
    const char* getError() const {
        return error.c_str();
    }
    virtual void close() {};
    /** 
     * @brief Reads @p ntoread characters from the stream and sets \a start to
     * the first character that was read.
     *
     * If @p ntoread is @c 0, then at least one character will be read.
     *
     * @param start Pointer passed by reference that will be set to point to
     *              the retrieved array of characters. If the end of the stream
     *              is encountered or an error occurs, the value of @p start
     *              is undefined.
     * @return the number of characters that were read. If 0 is returned, the
     *         end of the stream has been reached. If -1 is returned, an error
     *         has occured.
     **/
    virtual int32_t read(const T*& start) = 0;
    /** 
     * @brief Reads @p ntoread characters from the stream and sets \a start to
     * the first character that was read.
     *
     * If @p ntoread is @c 0, then at least one character will be read.
     *
     * @param start Pointer passed by reference that will be set to point to
     *              the retrieved array of characters. If the end of the stream
     *              is encountered or an error occurs, the value of @p start
     *              is undefined.
     * @param ntoread The number of characters to read from the stream. If
     *                @p is @c 0 the stream reads at least 1 character.
     * @return the number of characters that were read. If 0 is returned, the
     *         end of the stream has been reached. If -1 is returned, an error
     *         has occured.
     **/
    virtual int32_t read(const T*& start, int32_t ntoread) = 0;
    /* the available value may be greater than the actual value if
      the encoding is a variable one (such as utf8 or unicode) */
    /**
     * Skip @param ntoskip bytes. Unless an error occurs or the end of file is
     * encountered, this amount of bytes is skipped.
     * The optional @param skipped can be use to find out how many bites were skipped.
     * If the end of stream is reached, Eof is returned.
     * If an error occured, Error is returned.
     **/
    virtual StreamStatus skip(int64_t ntoskip, int64_t* skipped=0);
     /**
      * \short Marks the current position in this input stream.
      * A subsequent call to the reset method repositions this stream at the
      * last marked position so that subsequent reads re-read the same bytes.
      *
      * The readlimit arguments tells this input stream to allow that many
      * bytes to be read before the mark position gets invalidated.
      * The stream somehow remembers all the bytes read after the call to mark
      * and stands ready to supply those same bytes again if and whenever the
      * method reset is called. However, the stream is not required to remember
      * any data at all if more than readlimit bytes are read from the stream
      * before reset is called.
      *
      * When calling the method mark more than once at the same position in the
      * stream, the call with the largest value for \p readlimit is defining.
      **/
    virtual StreamStatus mark(int32_t readlimit) = 0;
      /**
       * \short Repositions this stream to the position at the time the mark
       * method was last called on this input stream.
       * The general contract of reset is:
       * - If the method mark has not been called since the stream was created,
       *   or the number of bytes read from the stream since mark was last
       *   called is larger than the argument to mark at that last call, then
       *   Error is returned.
       * - Otherwise the stream is reset to a state such that all the bytes
       *   read since the most recent call to mark (or since the start of the
       *   file, if mark has not been called) will be resupplied to subsequent
       *   callers of the read method, followed by any bytes that otherwise
       *   would have been the next input data as of the time of the call to
       *   reset.
       **/
    virtual StreamStatus reset() = 0;
};

/*template <class T>
StreamStatus
StreamBase<T>::read(T& c) {
    const T *buf;
    int32_t numRead;
    StreamStatus r;
    do {
        r = read(buf, numRead, 1);
    } while (r == Ok && numRead == 0);
    if (r == Ok) c = buf[0];
    return r;
}*/
/*
template <class T>
int32_t
StreamBase<T>::read(const T*& start, int32_t ntoread) {
    if (status == Eof) return 0;
    if (status == Error) return -1;
    int toread;
    const T* ptr;
    if (mark(ntoread) != Ok) return status;
    start = 0;
    do {
        numRead = read(ptr, numRead, ntoread);
        if (status != Ok) return status;
        if (start == 0) {
            start = ptr;
        }
        ntoread -= numRead;
    } while (ntoread);
    return status;
}
*/
template <class T>
StreamStatus
StreamBase<T>::skip(int64_t ntoskip, int64_t* skipped) {
    const T *begin;
    int32_t nread;
    if (skipped) *skipped = 0;
    while (ntoskip) {
        int32_t readstep = (int32_t)((ntoskip > INT32MAX) ?INT32MAX :ntoskip);
        nread = read(begin, readstep);
        if (status != Ok) return status;
        ntoskip -= nread;
        if (skipped) *skipped += nread;
    }
    return Ok;
}

} // end namespace jstreams

#endif
