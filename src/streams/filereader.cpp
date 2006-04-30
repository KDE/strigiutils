#include "jstreamsconfig.h"
#include "filereader.h"
#include "fileinputstream.h"
#include "inputstreamreader.h"
using namespace jstreams;

FileReader::FileReader(const char* fname, const char* encoding_scheme,
        int32_t cachelen, int32_t /*cachebuff*/) {
    input = new FileInputStream(fname, cachelen);
    reader = new InputStreamReader(input, encoding_scheme);
}
FileReader::~FileReader() {
    if (reader) delete reader;
    if (input) delete input;
}
int32_t
FileReader::read(const wchar_t*& start, int32_t min, int32_t max) {
    int32_t nread = reader->read(start, min, max);
    if (nread == -1) {
        error = reader->getError();
        status = Error;
        return -1;
    } else if (nread == 0) {
        status = Eof;
    }
    return nread;
}
int64_t
FileReader::mark(int32_t readlimit) {
    int64_t mp = reader->mark(readlimit);
    return mp;
}
StreamStatus
FileReader::reset() {
    status = reader->reset();
    if (status != Ok) error = reader->getError();
    return status;
}

