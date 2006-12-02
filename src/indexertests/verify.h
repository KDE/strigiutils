#ifndef VERIFY_H
#define VERIFY_H

#include "strigi_thread.h"

extern int founderrors;
extern StrigiMutex errorlock;

#define VERIFY(TESTBOOL) if (!(TESTBOOL)) {\
	fprintf(stderr, "test '%s' failed at\n\t%s:%i\n", \
		#TESTBOOL, __FILE__, __LINE__); \
        STRIGI_MUTEX_LOCK(&errorlock.lock); \
	founderrors++; \
        STRIGI_MUTEX_UNLOCK(&errorlock.lock); \
}

#endif
