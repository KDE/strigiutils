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
    if (file) {
        // allocate memory in the buffer
        mark(buffersize);
    }
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
/*char
FileInputStream::skip(int32_t ntoskip) {
    // if we want to skip further than we've already read,
    // use fseek
    // (this seems to not be a really relevant optimisation, for
    // moderately sized files
    char r = 0;
    if (buffer.avail >= ntoskip) {
        InputStream::skip(ntoskip);
    } else {
        int32_t skip = ntoskip - buffer.avail;
        printf("fseek %i %i\n", skip, errno);
        buffer.avail = 0;
        buffer.markPos = 0; // would become invalid after normal reads too
        buffer.curPos = buffer.start;
        r = fseek(file, skip, SEEK_CUR);
        printf("ftell %li %i\n", ftell(file), errno);
        exit(0);
    }
    return r;
}
*/

#include "inputstreambuffer.cpp"
