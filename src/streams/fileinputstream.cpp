#include "fileinputstream.h"
#include <cerrno>
#include <cstring>
using namespace jstreams;

const int32_t FileInputStream::defaultBufferSize = 1048576;
FileInputStream::FileInputStream(const char *filepath, int32_t buffersize) {
    // initialize values that signal state
    status = Ok;

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
        buffer.setSize(buffersize);
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
/*int32_t
FileInputStream::read(const char*& start, int32_t ntoread) {
    return 0;
}
int32_t
FileInputStream::read(const char*& start) {
    // if an error occured earlier, signal this
    if (status == Error) return -1;
    if (status == Eof) return 0;

    // if we cannot read and there's nothing in the buffer
    // (this can maybe be fixed by calling reset)
    if (file == NULL && buffer.avail == 0) return 0;

    // if buffer is empty, read from buffer
    if (buffer.avail == 0) {
        readFromFile();
        if (status) return status;
    }
    // set the pointers to the available data
    int32_t nread;
    buffer.read(start, nread);
    return nread;
}*/
void
FileInputStream::fillBuffer() {
    // prepare the buffer for writing
    int32_t bytesRead = buffer.getWriteSpace();
    // read into the buffer
    bytesRead = fread(buffer.curPos, 1, bytesRead, file);
    buffer.avail = bytesRead;
    // check the file stream status
    if (ferror(file)) {
        error = "Could not read from file '" + filepath + "'.";
        fclose(file);
        file = NULL;
        status = Error;
    } else if (feof(file)) {
        fclose(file);
        file = NULL;
        if (bytesRead == 0) {
            status = Ok;
        }
    }
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
