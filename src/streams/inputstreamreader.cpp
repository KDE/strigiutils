#include "inputstreamreader.h"
#include <cerrno>
using namespace jstreams;

InputStreamReader::InputStreamReader(StreamBase<char>* i, const char* enc) {
    status = Ok;
    finishedDecoding = false;
    input = i;
    if (sizeof(wchar_t) > 1) {
        converter = iconv_open("WCHAR_T", enc);
    } else {
        converter = iconv_open("ASCII", enc);
    }
    // check if the converter is valid
    if (converter == (iconv_t) -1) {
        error = "conversion from '";
        error += enc;
        error += "' not available.";
        status = Error;
        return;
    }
    charbuf.setSize(262);
    mark(262);
    charsLeft = 0;
}
InputStreamReader::~InputStreamReader() {
    if (converter != (iconv_t) -1) {
        iconv_close(converter);
    }
}
void
InputStreamReader::readFromStream() {
    // read data from the input stream
    inSize = input->read(inStart);
    if (inSize == 0) {
        status = Eof;
    } else if (inSize == -1) {
        status = Error;
    }
}
int32_t
InputStreamReader::decode(wchar_t* start, int32_t space) {
    // decode from charbuf
    char *inbuf = charbuf.readPos;
    size_t inbytesleft = charbuf.avail;
//    printf("decode %p %i %i\n", buffer.curPos, space, buffer.size);
    size_t outbytesleft = sizeof(wchar_t)*space;
    char *outbuf = (char*)start;
    size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    int32_t nwritten;
    if (r == (size_t)-1) {
        switch (errno) {
        case EILSEQ: //invalid multibyte sequence
//            printf("invalid multibyte\n");
            error = "Invalid multibyte sequence.";
            status = Error;
            return -1;
        case EINVAL: // last character is incomplete
            // move from inbuf to the end to the start of
            // the buffer
//            printf("last char incomplete %i %i\n", left, inbytesleft);
            memmove(charbuf.start, inbuf, inbytesleft);
            charbuf.readPos = charbuf.start;
            charbuf.avail = inbytesleft;
            nwritten = ((wchar_t*)outbuf) - start;
            break;
        case E2BIG: // output buffer is full
//            printf("output full read %i %i\n", read, charbuf.avail );
            charbuf.readPos += charbuf.avail - inbytesleft;
            charbuf.avail = inbytesleft;
            nwritten = space;
            break;
        default:
            exit(-1);
        }
    } else { //input sequence was completely converted
//        printf("complete\n" );
        charbuf.readPos = charbuf.start;
        charbuf.avail = 0;
        nwritten = ((wchar_t*)outbuf) - start;
        if (input == 0) {
            finishedDecoding = true;
        }
    }
    return nwritten;
}
int32_t
InputStreamReader::fillBuffer(wchar_t* start, int32_t space) {
//    printf("decodefromstream\n");
    // fill up charbuf
    if (charbuf.readPos == charbuf.start) {
 //       printf("fill up charbuf\n");
        const char *begin;
        int32_t numRead;
        numRead = input->read(begin, charbuf.size);
        switch (numRead) {
        case 0:
            // signal end of input buffer
            input = 0;
            if (charbuf.avail) {
                error = "stream ends on incomplete character";
                status = Error;
            } else {
                status = Eof;
            }
            return false;
        case -1:
            error = input->getError();
            status = Error;
            return false;
        default:
            // copy data into other buffer
            memmove(charbuf.start + charbuf.avail, begin, numRead);
            charbuf.avail = numRead;
            break;
        }
    }
    // decode
    return decode(start, space);
}
FileReader::FileReader(const char* fname, const char* /*encoding_scheme*/,
        int32_t cachelen, int32_t /*cachebuff*/) {
    input = new FileInputStream(fname, cachelen);
    reader = new InputStreamReader(input);
}
FileReader::~FileReader() {
    if (reader) delete reader;
    if (input) delete input;
}
int32_t
FileReader::read(const wchar_t*& start) {
    int32_t nread = reader->read(start);
    if (nread == -1) {
        error = reader->getError();
        status = Error;
        return -1;
    } else if (nread == 0) {
        status = Eof;
    }
    return nread;
}
int32_t
FileReader::read(const wchar_t*& start, int32_t ntoread) {
    int32_t nread = reader->read(start, ntoread);
    if (nread == -1) {
        error = reader->getError();
        status = Error;
        return -1;
    } else if (nread != ntoread) {
        status = Eof;
    }
    return nread;
}
StreamStatus
FileReader::mark(int32_t readlimit) {
    status = reader->mark(readlimit);
    if (status != Ok) error = reader->getError();
    return status;
}
StreamStatus
FileReader::reset() {
    status = reader->reset();
    if (status != Ok) error = reader->getError();
    return status;
}
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

