#ifndef SUBSTREAMPROVIDERPROVIDER_H
#define SUBSTREAMPROVIDERPROVIDER_H

namespace jstreams {

class SubStreamProvider;
template <class T> class StreamBase;
class SubStreamProviderProvider {
public:
SubStreamProviderProvider();
SubStreamProvider* getSubStreamProvider(StreamBase<char>* input);
};

}

#endif
