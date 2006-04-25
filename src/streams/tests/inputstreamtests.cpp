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
    if (s->getStatus() == jstreams::Error) {
        printf("error %s\n", s->getError());
    }
    QVERIFY(s->getStatus() == jstreams::Eof);
}

template <class T>
void
inputStreamTest2(StreamBase<T>* s) {
    int64_t p = s->getPosition();
    QVERIFY(s->mark(100) == Ok);
    QVERIFY(s->skip(100) > 0);
    QVERIFY(s->reset() == Ok);
    QVERIFY(s->getPosition() == p);
    inputStreamTest1(s);
}
void
subStreamProviderTest1(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    while (s) {
        inputStreamTest1<char>(s);
        s = ssp->nextEntry();
    }
    if (ssp->getStatus() == jstreams::Error) {
        printf("%s\n", ssp->getError());
    }
    QVERIFY(ssp->getStatus() == jstreams::Eof);
}
void
subStreamProviderTest2(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    while (s) {
        inputStreamTest2<char>(s);
        s = ssp->nextEntry();
    }
    if (ssp->getStatus() == jstreams::Error) {
        printf("%s\n", ssp->getError());
    }
    QVERIFY(ssp->getStatus() == jstreams::Eof);
}

int ninputstreamtests = 2;
void (*charinputstreamtests[])(StreamBase<char>*) = {
    inputStreamTest1, inputStreamTest2 };
void (*wcharinputstreamtests[])(StreamBase<wchar_t>*) = {
    inputStreamTest1, inputStreamTest2 };

int nstreamprovidertests = 2;
void (*streamprovidertests[])(SubStreamProvider*) = {
    subStreamProviderTest1, subStreamProviderTest2 };
