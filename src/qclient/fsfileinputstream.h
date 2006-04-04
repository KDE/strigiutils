#ifndef FSFILEINPUTSTREAM_H
#define FSFILEINPUTSTREAM_H

#include "inputstream.h"
#include "inputstreambuffer.h"

class QFSFileEngine;
class QString;

class FSFileInputStream : public InputStream {
private:
    bool open;
    QFSFileEngine *fse;
    InputStreamBuffer<char> buffer;

    void readFromFile();
public:
    static const size_t defaultBufferSize;
    FSFileInputStream(const QString &filename, size_t buffersize=defaultBufferSize);
    FSFileInputStream(QFSFileEngine *, size_t buffersize=defaultBufferSize);
    ~FSFileInputStream();
    Status reopen();
    Status read(const char*& start, size_t& nread, size_t max = 0);
    Status mark(size_t readlimit);
    Status reset();
};


#endif
