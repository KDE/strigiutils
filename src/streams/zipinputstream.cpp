#include "zipinputstream.h"
#include "gzipinputstream.h"
#include "subinputstream.h"
extern "C" {
    #include "dostime.h"
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
            uncompressionStream->restart(compressedEntryStream);
        } else {
            uncompressionStream = new GZipInputStream(compressedEntryStream,
                GZipInputStream::ZIPFORMAT);
        }
        uncompressedEntryStream
            = new SubInputStream(uncompressionStream, entryinfo.size);
    } else {
        uncompressedEntryStream = new SubInputStream(input, entryinfo.size);
    }
    return uncompressedEntryStream;
}
size_t
ZipInputStream::read2bytes(const unsigned char *b) {
    return b[0] + (b[1]<<8);
}
size_t
ZipInputStream::read4bytes(const unsigned char *b) {
    return b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
}
void
ZipInputStream::readHeader() {
    unsigned char hb[30];
    unsigned char *bptr;
    int toread;
    const char *begin;
    size_t nread;

    // read the first 30 characters
    toread = 30;
    bptr = hb;
    while (toread) {
        char r = input->read(begin, nread, toread);
        if (r) {
            status = -2;
            error = "Error reading header: " + input->getError();
            return;
        }
        memcpy(bptr, begin, nread);
        bptr += nread;
        toread -= nread;
    }
    // check the signature
    // check the first half of the signature
    if (hb[0] != 0x50 || hb[1] != 0x4b) {
        // signature is invalid
        status = -2;
        error = "Error: wrong zip signature.";
        return;
    }
    // check the second half of the signature
    if (hb[2] != 0x03 || hb[3] != 0x04) {
        // this may be the regular end of the file
        if (hb[2] != 0x01 || hb[3] != 0x02) {
            printf("This is new: %x %x %x %x\n", hb[0], hb[1], hb[2], hb[3]);
        }
        status = -1;
        return;
    }
    // read 2 bytes into the filename size
    size_t len = read2bytes(hb + 26);
    readFileName(len);
    if (status) {
        status = -2;
        error = "Error reading file name.";
        return;
    }
    // read 2 bytes into the length of the extra field
    len = read2bytes(hb + 28);
    char r = input->skip(len);
    if (r) {
        status = -2;
        error = "Error skipping extra field.";
        return;
    }
    // read 4 bytes into the length of the uncompressed size
    entryinfo.size = read4bytes(hb + 22);
    // read 4 bytes into the length of the compressed size
    entryCompressedSize = read4bytes(hb + 18);
    compressionMethod = read2bytes(hb+8);

    size_t generalBitFlags = read2bytes(hb+6);
    if (generalBitFlags & 8) { // is bit 3 set?
        // ohoh, the filesize and compressed file size are unknown at this point
        // in theory this is a solvable problem, but it's not easy:
        // one would need to keep a running crc32 and filesize and match it to the
        // data read so far
        status = -2;
        error = "This particular zip file format is not supported for reading "
            "as a stream.";
    }
    unsigned long dost = read4bytes(hb+10);
    entryinfo.mtime = dos2unixtime(dost);
}
void
ZipInputStream::readFileName(size_t len) {
    entryinfo.filename.resize(0);
    const char *begin;
    size_t nread;
    while (len) {
        char r = input->read(begin, nread, len);
        if (r) {
            status = -2;
            return;
        }
        entryinfo.filename.append(begin, nread);
        len -= nread;
    }
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
