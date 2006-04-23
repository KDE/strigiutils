#include "stringreader.h"
using namespace jstreams;

StringReader::StringReader(const wchar_t* value, const int32_t length ) {
    size = length;
    data = new wchar_t[size+1];
    size_t s = (size_t)(size*sizeof(wchar_t));
    memcpy(data, value, s);
}
StringReader::StringReader( const wchar_t* value ) {
    size = wcslen(value);
    data = new wchar_t[size+1];
    size_t s = (size_t)(size*sizeof(wchar_t));
    memcpy(data, value, s);
}
StringReader::~StringReader(){
    if (data) {
	delete data;
    }
}
int32_t
StringReader::read(const wchar_t*& start) {
    int32_t nread;
    if (size - position > INT32MAX) {
        nread = INT32MAX;
    } else {
        nread = (int32_t)(size - position);
    }
    if ( nread == 0 )
        return 0;
    start = data + position;
    position += nread;
    return nread;
}
int32_t
StringReader::read(const wchar_t*& start, int32_t ntoread) {
    if ( position >= size )
        return 0;
    int32_t nread = ntoread;
    if (nread > size) nread = (int32_t)size;
    start = data + position;
    position += nread;
    return nread;
}
StreamStatus
StringReader::mark(int32_t /*readlimit*/) {
    markpt = position;
    return Ok;
}
StreamStatus
StringReader::reset() {
    position = markpt;
    return Ok;
}

