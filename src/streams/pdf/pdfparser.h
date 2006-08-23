#ifndef PDFPARSER_H
#define PDFPARSER_H

#include "streambase.h"

class PdfParser {
public:
    class StreamHandler {
    public:
        virtual ~StreamHandler() {}
        virtual bool handle(jstreams::StreamBase<char>*) = 0;
    };
    class TextHandler {
    public:
        virtual ~TextHandler() {}
        virtual bool handle(const std::string&) = 0;
    };
private:
    const char* start;
    const char* end;
    const char* pos;
    int64_t objdefstart;
    jstreams::StreamBase<char>* stream;
    std::string error;

    // parsed objects
    double lastNumber;
    std::string lastName;
    std::string lastOperator;
    std::string lastString;
    void* lastObject;

    // event handlers
    StreamHandler* streamhandler;
    TextHandler* texthandler;

    jstreams::StreamStatus read(int32_t min, int32_t max);
    void forwardStream(jstreams::StreamBase<char>* s);
    jstreams::StreamStatus read2(int32_t min, int32_t max);
    jstreams::StreamStatus checkForData(int32_t m);
    bool isInString(char c, const char* s, int32_t n);
    jstreams::StreamStatus skipFromString(const char*str, int32_t n);
    jstreams::StreamStatus skipNotFromString(const char*str, int32_t n);
    jstreams::StreamStatus skipWhitespaceOrComment();
    jstreams::StreamStatus skipWhitespace();
    jstreams::StreamStatus skipKeyword(const char* str, int32_t len);
    jstreams::StreamStatus parseObjectStreamObject();
    jstreams::StreamStatus parseContentStreamObject();
    jstreams::StreamStatus parseComment();
    jstreams::StreamStatus parseBoolean();
    jstreams::StreamStatus parseNumber();
    jstreams::StreamStatus parseNumberOrIndirectObject();
    jstreams::StreamStatus parseLiteralString();
    jstreams::StreamStatus parseHexString();
    jstreams::StreamStatus parseName();
    jstreams::StreamStatus parseOperator();
    jstreams::StreamStatus parseDictionaryOrStream();
    jstreams::StreamStatus parseArray();
    jstreams::StreamStatus parseNull();
    jstreams::StreamStatus parseObjectStreamObjectDef();

    jstreams::StreamStatus handleSubStream(jstreams::StreamBase<char>*,
        const std::string& type, int32_t offset, int32_t n);
    jstreams::StreamStatus handleSubStream(jstreams::StreamBase<char>*,
        const std::string& type, int32_t offset, int32_t n, bool hasfilter,
        const std::string& filter);
    jstreams::StreamStatus parseObjectStream(jstreams::StreamBase<char>*,
        int32_t offset, int32_t n);
    jstreams::StreamStatus parseContentStream(jstreams::StreamBase<char>*);
public:
    PdfParser();
    jstreams::StreamStatus parse(jstreams::StreamBase<char>* s);
    const std::string& getError() { return error; }
    void setStreamHandler(StreamHandler* handler) { streamhandler = handler; }
    void setTextHandler(TextHandler* handler) { texthandler = handler; }
};

#endif
