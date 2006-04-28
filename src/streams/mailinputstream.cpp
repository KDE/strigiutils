#include "jstreamsconfig.h"
#include "mailinputstream.h"
#include "subinputstream.h"
using namespace jstreams;
using namespace std;

namespace jstreams {
class SubMailStream : public StreamBase<char> {
public:
    SubMailStream(MailInputStream*){}
    int32_t read(const char*& start, int32_t min=0, int32_t max=0) {return 0;}
    StreamStatus mark(int32_t readlimit) { return Error;}
    StreamStatus reset() {return Error;}
};
}

/**
 * Very naive mail detection. An file that starts with 'Received:' or 'From:'
 * must be an email.
 **/
bool
MailInputStream::checkHeader(const char* data, int32_t datasize) {
    static char* from = "From: ";
    static char* receiver = "Received: ";
    int fromlength = (datasize > 6) ?6: datasize;
    int receiverlength = (datasize > 10) ?10: datasize;
    return memcmp(from, data, fromlength) == 0
        || memcmp(receiver, data, receiverlength) == 0;
}
MailInputStream::MailInputStream(StreamBase<char>* input)
        : SubStreamProvider(input), entrystream(0) {
//    printf("%p\n", input);
    linenum = 0;
    skipHeader();

    // get the boundary
    const char* ct = contenttype.c_str();
    const char* battr = strcasestr(ct, "boundary=");
    if (battr == 0) {
        // so far we just scan for a bountdary attribute
        return;
    }
    battr += 9;
    const char* bend = strchr(battr, ';');
    if (bend == 0) {
        bend = ct + contenttype.length();
    }
    if (*battr == '"') {
        boundary = string(battr+1, bend-battr-2);
    } else {
        boundary = string(battr, bend-battr);
    }
}
MailInputStream::~MailInputStream() {
    if (entrystream) {
        delete entrystream;
    }
}
void
MailInputStream::readLine() {
    if (bufstart == 0) return;
    linenum++;
    linestart = lineend;
    bool backslashr = false;
    if (eol) {
        linestart++; // skip \r or \n
        backslashr = *lineend == '\r';
        //printf("%p %p %p %p\n", linestart, lineend, bufstart, bufend);
        if (backslashr && linestart != bufend) printf("%i\n", *linestart);
        if (backslashr && linestart != bufend && *linestart == '\n') {
            // skip \n of \r\n
            linestart++;
        }
        lineend = linestart;
    }
    while (lineend != bufend && *lineend != '\n' && *lineend != '\r') {
        lineend++;
    }
    eol = true;
    if (lineend == bufend) {
        fillBuffer();
        if (bufstart == 0) {
            // the input has been exhausted
            return;
        }
        if (backslashr && *linestart == '\n') {
            // we have to skip a \n because of a \r\n lineend across the read
            // boundary
            linestart++;
            if (linestart == bufend) {
                fillBuffer();
                if (bufstart == 0) {
                    // the input has been exhausted
                    return;
                }
            }
        }
        lineend = linestart;
        while (lineend != bufend && *lineend != '\n' && *lineend != '\r') {
            lineend++;
        }
        if (lineend == bufend) {
            string str(linestart, 10);
            printf("line %i is too long '%s' %i %i\n", linenum, str.c_str(),
                lineend-linestart, maxlinesize);
            eol = false;
        }
    }
}
void
MailInputStream::fillBuffer() {
    input->reset();
    input->skip(linestart-bufstart);
    input->mark(maxlinesize);
    int32_t nread = input->read(bufstart, maxlinesize, 0);
    if (nread > 0) {
        bufend = bufstart + nread;
        linestart = bufstart;
    } else {
        bufstart = 0;
    }
}
void
MailInputStream::skipHeader() {
    maxlinesize = 100;
    input->mark(maxlinesize);
    int32_t nread = input->read(bufstart, maxlinesize, 0);
    if (nread <= 0) {
        // error: file too short
        return;
    }
    lastHeader = 0;
    eol = false;
    bufend = bufstart + nread;
    lineend = bufstart;
    readLine();
    while (bufstart) {
        readLine();
        if (linestart == lineend) {
            return;
        }
        handleHeaderLine(linestart, lineend);
    }
}
void
MailInputStream::scanBody() {

    while (bufstart) {
        readLine();
        if (handleBodyLine()) break;
    }

//    const char* bend = strchr(bstart+1, '"');
 //   string boundary(bstart+1, bend-bstart-2);
//    printf("%s\n", contenttype.c_str());
//    printf("%s\n", boundary.c_str());
//    printf("%s %i\n", boundary.c_str(), n);
}
void
MailInputStream::handleHeaderLine(const char* start, const char* end) {
    static const char* subject = "Subject:";
    static const char* contenttype = "Content-Type:";
    if (end-start < 2) return;
    if (lastHeader && isspace(*start)) {
        *lastHeader += string(start, end-start);
    } else if (end-start < 8) {
        lastHeader = 0;
        return;
    } else if (strncasecmp(start, subject, 8) == 0) {
        this->subject = std::string(start, end-start);
        lastHeader = &this->subject;
    } else if (strncasecmp(start, contenttype, 13) == 0) {
        this->contenttype = std::string(start, end-start);
        lastHeader = &this->contenttype;
    } else {
        lastHeader = 0;
    }
}
// return true if we are at the start of a base64 encoded block
bool
MailInputStream::handleBodyLine() {
    if (boundary.length()+2 != size_t(lineend-linestart)) {
        return false;
    }
    int n = strncmp(boundary.c_str(), linestart+2, boundary.length());
    bool base64 = false;
    if (n == 0) {
        // start of new block
        // skip header
        do {
            readLine();
            if (lineend - linestart > 6
                    && strncasecmp("base64", lineend-6, 6) == 0) {
                base64 = true;
            }
        } while (bufstart && linestart != lineend);
    }
    if (base64) {
	entrystream = new SubMailStream(this);
        printf("new stream\n");
    }
    return base64;
}
bool
MailInputStream::lineIsEndOfBlock() {
    //printf("%i %i '%s' '*s'\n", boundary.length()+4, lineend-linestart,
    //    boundary.c_str());//, lineend-linestart, linestart);
    return boundary.length()+4 == (size_t)(lineend-linestart)
        && strncmp(boundary.c_str(), linestart, lineend-linestart);
}
StreamBase<char>*
MailInputStream::nextEntry() {
    delete entrystream;
    entrystream = 0;
    if (status) return 0;
    scanBody();
    return entrystream;
}

