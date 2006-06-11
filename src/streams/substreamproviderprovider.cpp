#include "substreamproviderprovider.h"
#include "mailinputstream.h"
#include "tarinputstream.h"
#include "zipinputstream.h"
using namespace jstreams;

SubStreamProviderProvider::SubStreamProviderProvider() {
}
SubStreamProvider*
SubStreamProviderProvider::getSubStreamProvider(StreamBase<char>* input) {
    // read the header
    const char* header;
    int64_t pos = input->getPosition();
    int32_t headersize = input->read(header, 1024, 0);
    input->reset(pos);
    if (headersize <= 0) {
        return 0;
    }
    printf("%x%x%x%x\n", header[0], header[1], header[2], header[3]);
    if (MailInputStream::checkHeader(header, headersize)) {
        return new MailInputStream(input);
    }
    if (ZipInputStream::checkHeader(header, headersize)) {
        printf("zip!\n");
        return new ZipInputStream(input);
    }
    printf("no match %i\n", headersize);
    return 0;
}
