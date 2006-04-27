#include "jstreamsconfig.h"
#include "mailinputstream.h"
#include "subinputstream.h"
using namespace jstreams;
using namespace std;

namespace jstreams {
class Base64InputStream : public StreamBase<char> {
private:
    const char* pos;
    MailInputStream* source;
    static const char* alphabet;
    static bool inalphabet[256];
    static char decoder[133];
public:
    Base64InputStream(MailInputStream*);
    int32_t fillBuffer(char* start, int32_t space);
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
    scanBody();
}
MailInputStream::~MailInputStream() {
    if (entrystream) {
        delete entrystream;
    }
}
void
MailInputStream::readLine() {
    linenum++;
    linestart = lineend;
    bool backslashr = false;
    if (eol) {
        linestart++; // skip \r or \n
        backslashr = *lineend == '\r';
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
MailInputStream::skipHeader() {
    maxlinesize = 100;
    input->mark(maxlinesize);
    int32_t nread = input->read(bufstart, maxlinesize);
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
MailInputStream::fillBuffer() {
    input->reset();
    input->skip(linestart-bufstart);
    input->mark(maxlinesize);
    int32_t nread = input->readAtLeast(bufstart, maxlinesize);
    if (nread > 0) {
        bufend = bufstart + nread;
        linestart = bufstart;
    } else {
        bufstart = 0;
    }
}
void
MailInputStream::scanBody() {
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

    int n = 0;
    while (bufstart) {
        readLine();
        if (handleBodyLine()) n++;
    }

//    const char* bend = strchr(bstart+1, '"');
 //   string boundary(bstart+1, bend-bstart-2);
//    printf("%s\n", contenttype.c_str());
//    printf("%s\n", boundary.c_str());
    printf("%s %i\n", boundary.c_str(), n);
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
    if (boundary.length()+2 != lineend-linestart) {
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
    return base64;
}
StreamBase<char>*
MailInputStream::nextEntry() {
    if (status) return 0;
    return entrystream;
}
Base64InputStream::Base64InputStream(MailInputStream*s) :pos(s->bufend),
        source(s) {
    for (int i=64; i<256; ++i) {
        inalphabet[i] = 0;
    }
    for (int i=0; i<64; ++i) {
	inalphabet[alphabet[i]] = true;
	decoder[alphabet[i]] = i;
    }
}
int32_t
Base64InputStream::fillBuffer(char* start, int32_t space) {
    if (pos == source->bufend) {
        source->readLine();
        if (source->linestart == source->lineend) {
            return 0;
        }
    }
    return 0;
}
const char *Base64InputStream::alphabet
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
bool Base64InputStream::inalphabet[256];
char Base64InputStream::decoder[133];

namespace jstreams {
/**
 * Class for string search that uses the Knuth-Morris-Pratt algorithm.
 * Code based on the example on
 * http://en.wikipedia.org/wiki/Knuth-Morris-Pratt_algorithm
 **/
class KMPSearcher {
private:
    string query;
    int32_t* table;
    int32_t len;
    int32_t maxlen;
public:
    KMPSearcher() :table(0) { }
    ~KMPSearcher() {
        if (table) {
            free(table);
        }
    }
    void setQuery(const string& );
    const char* search(const char* haystack, int32_t haylen) const;
};
}
void
KMPSearcher::setQuery(const string& query) {
    this->query = query;
    int32_t len = query.length();
    const char* p = query.c_str();
    if (table) {
        if (len > maxlen) {
            table = (int32_t*)realloc(table, sizeof(int32_t)*(len+1));
            maxlen = len;
        }
    } else {
        table = (int32_t*)malloc(sizeof(int32_t)*(len+1));
        maxlen = len;
    }
    int32_t i = 0;
    int32_t j = -1;
    char c = '\0';

    // build the mismatch table    
    table[0] = j;
    while (i < len) {
        if (p[i] == c) {
            table[i + 1] = j + 1;
            ++j;
            ++i;
        } else if (j > 0) {
            j = table[j];
        } else {
            table[i + 1] = 0;
            ++i;
            j = 0;
        }
        c = p[j];
    }
}
const char*
KMPSearcher::search(const char* haystack, int32_t haylen) const {
    if (table == 0) return 0;
    const char* needle = query.c_str();
    // search for the pattern
    int32_t i = 0;
    int32_t j = 0;
    while (j + i < haylen && i < len) {
        if (haystack[j + i] == needle[i]) {
            ++i;
        } else {
            j += i - table[i];
            if (i > 0) i = table[i];
        }
    }
    
    if (needle[i] == '\0') {
        return haystack + j;
    } else {
        return 0;
    }
}

