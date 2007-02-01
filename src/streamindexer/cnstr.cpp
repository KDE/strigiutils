#include "cnstr.h"

int cnstr::numRefs = 0;
std::map<const char *, int, cnstr::ltstr> cnstr::strings;
std::map<const char *, int, cnstr::ltstr>::iterator cnstr::nullpos
    = cnstr::strings.end();
cnstr cnstr::null(0);
cnstr cnstr::empty("");
