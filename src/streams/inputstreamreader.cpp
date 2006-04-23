#include "jstreamsconfig.h"
#include "inputstreamreader.h"
#include "jstreamsconfig.h"
#include <cerrno>
using namespace jstreams;

InputStreamReader::InputStreamReader(StreamBase<char>* i, const char* enc) {
    status = Ok;
    finishedDecoding = false;
    input = i;
    if (enc == 0) enc = "UTF8";
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
    ICONV_CONST char *inbuf = charbuf.readPos;
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
    printf("fillBuffer\n");
    // fill up charbuf
    if (input && charbuf.readPos == charbuf.start) {
 //       printf("fill up charbuf\n");
        const char *begin;
        int32_t numRead;
        numRead = input->read(begin, charbuf.size - charbuf.avail);
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
            return -1;
        case -1:
            input = 0;
            error = input->getError();
            status = Error;
            return -1;
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
