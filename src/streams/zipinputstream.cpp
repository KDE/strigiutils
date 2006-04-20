#include "zipinputstream.h"
#include "gzipinputstream.h"
#include "subinputstream.h"
extern "C" {
    #include "dostime.h"
}
using namespace jstreams;

bool
ZipInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x50, 0xb4, 0x03, 0x04};
    if (datasize < 4) return false;
    return memcmp(data, magic, 4) == 0;
}
ZipInputStream::ZipInputStream(InputStream *input)
        : SubStreamProvider(input) {
    compressedEntryStream = 0;
    uncompressionStream = 0;
    uncompressedEntryStream = 0;
}
ZipInputStream::~ZipInputStream() {
    if (compressedEntryStream) {
        delete compressedEntryStream;
    }
    if (uncompressionStream) {
        delete uncompressionStream;
    }
    if (uncompressedEntryStream) {
        delete uncompressedEntryStream;
    }
}
SubInputStream*
ZipInputStream::nextEntry() {
    if (status) return 0;
    // clean up the last stream(s)
    if (uncompressedEntryStream) {
        if (compressedEntryStream) {
            compressedEntryStream->skipToEnd();
            delete compressedEntryStream;
            compressedEntryStream = 0;
            delete uncompressionStream;
            uncompressionStream = 0;
        } else {
            uncompressedEntryStream->skipToEnd();
        }
        delete uncompressedEntryStream;
        uncompressedEntryStream = 0;
    }
    readHeader();
    if (status) return 0;
    if (compressionMethod == 8) {
        compressedEntryStream = new SubInputStream(input, entryCompressedSize);
        if (uncompressionStream) {
            delete uncompressionStream;
        }
        uncompressionStream = new GZipInputStream(compressedEntryStream,
                GZipInputStream::ZIPFORMAT);
        uncompressedEntryStream
            = new SubInputStream(uncompressionStream, entryinfo.size);
    } else {
        uncompressedEntryStream = new SubInputStream(input, entryinfo.size);
    }
    return uncompressedEntryStream;
}
int32_t
ZipInputStream::read2bytes(const unsigned char *b) {
    return b[0] + (b[1]<<8);
}
int32_t
ZipInputStream::read4bytes(const unsigned char *b) {
    return b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
}
void
ZipInputStream::readHeader() {
    const unsigned char *hb;
    const char *b;
    int32_t toread;
    int32_t nread;

    // read the first 30 characters
    toread = 30;
    nread = input->read(b, toread);
    if (nread != toread) {
        error = "Error reading LongLink: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        return;
    }
    hb = (const unsigned char*)b;
    // check the signature
    // check the first half of the signature
    if (hb[0] != 0x50 || hb[1] != 0x4b) {
        // signature is invalid
        status = Error;
        error = "Error: wrong zip signature.";
        return;
    }
    // check the second half of the signature
    if (hb[2] != 0x03 || hb[3] != 0x04) {
        // this may be the regular end of the file
        if (hb[2] != 0x01 || hb[3] != 0x02) {
            printf("This is new: %x %x %x %x\n", hb[0], hb[1], hb[2], hb[3]);
        }
        status = Eof;
        return;
    }
    // read 2 bytes into the filename size
    int32_t filenamelen = read2bytes(hb + 26);
    int32_t extralen = read2bytes(hb + 28);
    // read 4 bytes into the length of the uncompressed size
    entryinfo.size = read4bytes(hb + 22);
    // read 4 bytes into the length of the compressed size
    entryCompressedSize = read4bytes(hb + 18);
    compressionMethod = read2bytes(hb + 8);
    int32_t generalBitFlags = read2bytes(hb+6);
    if (generalBitFlags & 8) { // is bit 3 set?
        // ohoh, the filesize and compressed file size are unknown at this point
        // in theory this is a solvable problem, but it's not easy:
        // one would need to keep a running crc32 and filesize and match it to the
        // data read so far
        status = Error;
        error = "This particular zip file format is not supported for reading "
            "as a stream.";
    }
    unsigned long dost = read4bytes(hb+10);
    entryinfo.mtime = dos2unixtime(dost);

    readFileName(filenamelen);
    if (status) {
        status = Error;
        error = "Error reading file name.";
        return;
    }
    // read 2 bytes into the length of the extra field
    StreamStatus r = input->skip(extralen);
    if (r) {
        status = Error;
        error = "Error skipping extra field.";
        return;
    }
}
void
ZipInputStream::readFileName(int32_t len) {
    entryinfo.filename.resize(0);
    const char *begin;
    int32_t nread = input->read(begin, len);
    if (nread != len) {
        error = "Error reading filename: ";
        if (nread == -1) {
            error += input->getError();
        } else {
            error += " premature end of file.";
        }
        return;
    }
    entryinfo.filename.append(begin, nread);

    // temporary hack for determining if this is a directory:
    // does the filename end in '/'?
    len = entryinfo.filename.length();
    if (entryinfo.filename[len-1] == '/') {
        entryinfo.filename.resize(len-1);
        entryinfo.type = EntryInfo::Dir;
    } else {
        entryinfo.type = EntryInfo::File;
    }
}
