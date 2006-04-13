#ifndef TARINPUTSTREAM_H
#define TARINPUTSTREAM_H

#include "substreamprovider.h"

/**
 * Implementation of SubStreamProvider for reading the tar format. The InputStream recognizes this format:
 * - tar archive is composed of blocks of 512 bytes. Every file and every header starts at an offset
 *   of a multiple of 512.
 * - each file starts with a header of size 512.
 * - up to 100 bytes of filename at offset 0 of file header
 * - file size is given in 7 bytes of octal format starting at position 124 of the file header
 * - if the filename is '././@LongLink' then this block contains the length of the file name at
 *   position 124. The next blocks contain the filename. The block after that is the real header
 *   with the file size and only then does the file contents start.
 *
 * http://www.gnu.org/software/tar/manual/html_node/Standard.html
 **/

namespace jstreams {

class TarInputStream : public SubStreamProvider {
private:
    // information relating to the current entry
    SubInputStream *output;
    int32_t numPaddingBytes;

    void readFileName(int32_t len);
    const char* readHeader();
    void parseHeader();
    int32_t readOctalField(const char *b, int32_t offset);
    void readLongLink(const char *b);
public:
    TarInputStream(InputStream *input);
    ~TarInputStream();
    SubInputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
};

} // end namespace jstreams

#endif
