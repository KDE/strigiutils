#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <string>

namespace jstreams {

bool checkUtf8(const char* p, int32_t length);

bool checkUtf8(const std::string&);

void convertNewLines(char* p);

}

#endif
