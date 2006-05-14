#ifndef MAILINPUTSTREAM_H
#define MAILINPUTSTREAM_H

#include "substreamprovider.h"

/**
 * This is a proof of concept implementation for handling email streams as
 * archives. It allows one to read the email body and email attachements as
 * streams.
 **/

namespace jstreams {
class StringTerminatedSubStream;
class MailInputStream : public SubStreamProvider {
private:
    // variables that record the current read state
    bool eol; // true if the buffer contains a line end
    int64_t bufstartpos;
    int linenum;
    int maxlinesize;
    const char* linestart;
    const char* lineend;
    const char* bufstart;
    const char* bufend;

    StreamBase<char>* entrystream;
    StringTerminatedSubStream* substream;
    std::string subject;
    std::string contenttype;
    std::string contenttransferencoding;
    std::string contentdisposition;
    std::string* lastHeader;

    std::string boundary;

    void readLine();
    void fillBuffer();
    void skipHeader();
    void scanBody();
    void handleHeaderLine();
    void handleBodyLine();
    bool lineIsEndOfBlock();
    void rewindToLineStart();
    bool checkHeaderLine() const;
    void clearHeaders();
    std::string getValue(const char* n, const std::string& headerline) const;
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
