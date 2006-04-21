#include "fileinputstream.h"
#include <cerrno>
#include <cstring>
using namespace jstreams;

const int32_t FileInputStream::defaultBufferSize = 1048576;
FileInputStream::FileInputStream(const char *filepath, int32_t buffersize) {

    // try to open the file for reading
    file = fopen(filepath, "rb");
    this->filepath = filepath;
    if (file == 0) {
        // handle error
        error = "Could not read file '";
        error += filepath;
        error += "': ";
	error += strerror(errno);
        status = Error;
        return;
    }
    // determine file size. if the stream is not seekable, the size will be -1
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // allocate memory in the buffer
    mark(buffersize);
}
FileInputStream::~FileInputStream() {
    if (file) {
        if (fclose(file)) {
            // handle error
            error = "Could not close file '" + filepath + "'.";
        }
    }
}
int32_t
FileInputStream::fillBuffer(char* start, int32_t space) {
    if (file == 0) return -1;
    // read into the buffer
    int32_t nwritten = fread(start, 1, space, file);
    // check the file stream status
    if (ferror(file)) {
        error = "Could not read from file '" + filepath + "'.";
        fclose(file);
        file = 0;
        status = Error;
        return -1;
    }
    if (feof(file)) {
        fclose(file);
        file = 0;
    }
    return nwritten;
}

#include "inputstreambuffer.cpp"
