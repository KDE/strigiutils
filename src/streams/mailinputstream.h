#ifndef MAILINPUTSTREAM_H
#define MAILINPUTSTREAM_H

#include "substreamprovider.h"

/**
 * This is a proof of concept implementation for handling email streams as
 * archives. It allows one to read the email body and email attachements as
 * streams.
 **/

namespace jstreams {
class Base64InputStream;
class MailInputStream : public SubStreamProvider {
friend class Base64InputStream;
private:
    // variables that record the current read state
    bool firstline;
    bool eol; // true if the buffer contains a line end
    int linenum;
    int maxlinesize;
    const char* linestart;
    const char* lineend;
    const char* bufstart;
    const char* bufend;

    StreamBase<char>* entrystream;
    std::string subject;
    std::string contenttype;
    std::string* lastHeader;

    std::string boundary;

    void readLine();
    void fillBuffer();
    void skipHeader();
    void scanBody();
    void handleHeaderLine(const char* start, const char* end);
    bool handleBodyLine();
public:
    MailInputStream(StreamBase<char>* input);
    ~MailInputStream();
    StreamBase<char>* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    const std::string& getSubject() { return subject; }
    const std::string& getContentType() { return contenttype; }
};

} // end namespace jstreams

#endif
