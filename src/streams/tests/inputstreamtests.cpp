#include "inputstreamtests.h"
#include "../substreamprovider.h"
#include <QtTest/QtTest>
using namespace jstreams;

template <class T>
void
inputStreamTest1(StreamBase<T>* s) {
    int64_t size = s->getSize();
    const T* ptr;
    int32_t n;
    if (size == -1) {
        n = 0;
        int32_t n2 = s->read(ptr);
        while (n2 > 0) {
            n += n2;
            n2 = s->read(ptr);
        }
    } else {
        // read past the end
        n = s->read(ptr, size+1);
    }
    QVERIFY(size == -1 || size == n);
    QVERIFY(s->getPosition() == n);
    QVERIFY(s->getStatus() == jstreams::Eof);
}

template <class T>
void
inputStreamTest2(StreamBase<T>* s) {
}
void
subStreamProviderTest1(SubStreamProvider* ssp) {
    SubInputStream *s = ssp->nextEntry();
    while (s) {
        inputStreamTest1<char>(s);
        s = ssp->nextEntry();
    }
    if (ssp->getStatus() == jstreams::Error) {
        printf("%s\n", ssp->getError());
    }
    QVERIFY(ssp->getStatus() == jstreams::Eof);
}
