#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <string>

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
class InputStream {
public:
    enum Status {Ok, Eof, Error};
protected:
    std::string error;
    Status status; // 0: all ok, -1: EOF, -2 error
public:
    virtual ~InputStream() {}
    /**
     * Return a string representation of the last error that has occurred.
     * If no error has occurred, an empty string is returned.
     **/
    const std::string& getError() const {
        return error;
    }
    /**
     * Reads input data and points the input pointers to the read
     * data.
     * If the end of stream is reached, Eof is returned.
     * If an error occured, Error is returned.
     * No more than @param max data wil be read. If @param max == 0, there's no limit.
     **/
    virtual Status read(const char*& start, size_t& read, size_t max = 0) = 0;
    /**
     * Skip @param ntoskip bytes. Unless an error occurs or the end of file is
     * encountered, this amount of bytes is skipped.
     * If the end of stream is reached, Eof is returned.
     * If an error occured, Error is returned.
     **/
    virtual Status skip(size_t ntoskip);
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
    virtual Status mark(size_t readlimit) = 0;
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
    virtual Status reset() = 0;
};

#endif
