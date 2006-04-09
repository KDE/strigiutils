#include "Reader.h"
#include <cerrno>

Reader::Status
Reader::read(wchar_t& c) {
    const wchar_t *buf;
    int32_t numRead;
    Status r;
    do {
        r = read(buf, numRead, 1);
    } while (r == Ok && numRead == 0);
    if (r == Ok) c = buf[0];
    return r;
}
Reader::Status
Reader::skip(int32_t ntoskip) {
    const wchar_t *begin;
    int32_t nread;
    while (ntoskip) {
        Status r = read(begin, nread, ntoskip);
        if (r != Ok) return r;
        ntoskip -= nread;
    }
    return Ok;
}
InputStreamReader::InputStreamReader(InputStream *i, const char* enc) {
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
    buffer.setSize(262);
    charsLeft = 0;
}
InputStreamReader::~InputStreamReader() {
    if (converter != (iconv_t) -1) {
        iconv_close(converter);
    }
}
Reader::Status
InputStreamReader::read(const wchar_t*& start, int32_t& nread, int32_t max) {
    // if an error occured earlier, signal this
    if (status) return status;

    // if we cannot read and there's nothing in the buffer
    // (this can maybe be fixed by calling reset)
    if (finishedDecoding && buffer.avail == 0) return Eof;

    // check if there is still data in the buffer
    if (buffer.avail == 0) {
        decodeFromStream();
        if (status) return status;
    }

    // set the pointers to the available data
    buffer.read(start, nread, max);
    return Ok;
}
void
InputStreamReader::readFromStream() {
    // read data from the input stream
    InputStream::Status s = input->read(inStart, inSize);
    switch (s) {
    case InputStream::Ok:
        status = Ok; break;
    case InputStream::Eof:
        status = Eof; break;
    case InputStream::Error:
        status = Error; break;
    }
}
void
InputStreamReader::decode() {
    // decode from charbuf
    char *inbuf = charbuf.curPos;
    size_t inbytesleft = charbuf.avail;
    int32_t space = buffer.getWriteSpace();
//    printf("decode %p %i %i\n", buffer.curPos, space, buffer.size);
    size_t outbytesleft = sizeof(wchar_t)*space;
    char *outbuf = (char*)buffer.curPos;
    size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    if (r == (size_t)-1) {
        switch (errno) {
        case EILSEQ: //invalid multibyte sequence
//            printf("invalid multibyte\n");
            error = "Invalid multibyte sequence.";
            status = Error;
            return;
        case EINVAL: // last character is incomplete
            // move from inbuf to the end to the start of
            // the buffer
//            printf("last char incomplete %i %i\n", left, inbytesleft);
            memmove(charbuf.start, inbuf, inbytesleft);
            charbuf.curPos = charbuf.start;
            charbuf.avail = inbytesleft;
            buffer.avail = ((wchar_t*)outbuf) - buffer.curPos;
            break;
        case E2BIG: // output buffer is full
//            printf("output full read %i %i\n", read, charbuf.avail );
            charbuf.curPos += charbuf.avail - inbytesleft;
            charbuf.avail = inbytesleft;
            buffer.avail = space;
            break;
        default:
            exit(-1);
        }
    } else { //input sequence was completely converted
//        printf("complete\n" );
        charbuf.curPos = charbuf.start;
        charbuf.avail = 0;
        buffer.avail = ((wchar_t*)outbuf) - buffer.curPos;
        if (input == 0) {
            finishedDecoding = true;
        }
    }
}
void
InputStreamReader::decodeFromStream() {
//    printf("decodefromstream\n");
    // fill up charbuf
    if (charbuf.curPos == charbuf.start) {
 //       printf("fill up charbuf\n");
        const char *begin;
        int32_t numRead;
        InputStream::Status s;
        s = input->read(begin, numRead, charbuf.size);
        switch (s) {
        case InputStream::Ok:
            // copy data into other buffer
            bcopy(begin, charbuf.start + charbuf.avail, numRead);
            charbuf.avail = numRead;
            break;
        case InputStream::Eof:
            // signal end of input buffer
            input = 0;
            if (charbuf.avail) {
                error = "stream ends on incomplete character";
                status = Error;
            } else {
                status = Eof;
            }
            return;
        case InputStream::Error:
            error = input->getError();
            status = Error;
            return;
        }
    }
    // decode
    decode();
}
Reader::Status
InputStreamReader::mark(int32_t readlimit) {
    buffer.mark(readlimit);
    return Ok;
}
Reader::Status
InputStreamReader::reset() {
    if (buffer.markPos) {
        buffer.reset();
        return Ok;
    } else {
        error = "No valid mark for reset.";
        return Error;
    }
}
FileReader::FileReader(const char* fname, const char* encoding_scheme,
        int32_t cachelen, int32_t cachebuff) {
    input = new FileInputStream(fname, cachelen);
    reader = new InputStreamReader(input);
}
FileReader::~FileReader() {
    if (reader) delete reader;
    if (input) delete input;
}
Reader::Status
FileReader::read(const wchar_t*& start, int32_t& nread, int32_t max) {
    status = reader->read(start, nread, max);
    if (status != Ok) error = reader->getError();
    return status;
}
Reader::Status
FileReader::mark(int32_t readlimit) {
    status = reader->mark(readlimit);
    if (status != Ok) error = reader->getError();
    return status;
}
Reader::Status
FileReader::reset() {
    status = reader->reset();
    if (status != Ok) error = reader->getError();
    return status;
}
StringReader::StringReader(const wchar_t* value, const int32_t length )
        : len(length) {
    this->data = new wchar_t[len+1];
    bcopy(value, data, len*sizeof(wchar_t));
    this->pt = 0;
}
StringReader::StringReader( const wchar_t* value ) {
    this->len = wcslen(value);
    bcopy(value, data, len*sizeof(wchar_t));
    this->pt = 0;
}
StringReader::~StringReader(){
    close();
}
Reader::Status
StringReader::read(const wchar_t*& start, int32_t& nread, int32_t max) {
    if ( pt >= len )
        return Eof;
    nread = max;
    if (nread > len) nread = len;
    start = data + pt;
    return Ok;
}
Reader::Status
StringReader::read(wchar_t&c) {
    if (pt == len) {
        return Eof;
    }
    c = data[pt++];
    return Ok;
}
void
StringReader::close(){
    if (data) {
        delete data;
    }
}
Reader::Status
StringReader::mark(int32_t /*readlimit*/) {
    markpt = pt;
    return Ok;
}
Reader::Status
StringReader::reset() {
    pt = markpt;
    return Ok;
}

#include "inputstreambuffer.cpp"


