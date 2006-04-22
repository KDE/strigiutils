#ifndef INPUTSTREAMTESTS
#define INPUTSTREAMTESTS

namespace jstreams {
    template <class T>
    class StreamBase;
    class SubStreamProvider;
}

template <class T>
void inputStreamTest1(jstreams::StreamBase<T>* stream);

template <class T>
void inputStreamTest2(jstreams::StreamBase<T>* stream);

void subStreamProviderTest1(jstreams::SubStreamProvider* stream);

#endif
