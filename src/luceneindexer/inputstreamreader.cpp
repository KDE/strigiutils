#include "inputstreamreader.h"
#include "inputstream.h"
#include <cassert>

InputStreamReader::InputStreamReader(InputStream *input) {
    // the current implementation can only handle characters of 1 byte
    assert(sizeof(TCHAR)==1);
    this->input = input;
    pos = 0;
}
void
InputStreamReader::close() {
}
int64_t
InputStreamReader::position() {
    return pos;
}
Status
InputStreamReader::read(const TCHAR*& start, int32_t& nread, int32_t max) {
}
