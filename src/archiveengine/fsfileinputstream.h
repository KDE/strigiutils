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
    static const int32_t defaultBufferSize;
    FSFileInputStream(const QString &filename, int32_t buffersize=defaultBufferSize);
    FSFileInputStream(QFSFileEngine *, int32_t buffersize=defaultBufferSize);
    ~FSFileInputStream();
    Status reopen();
    Status read(const char*& start, int32_t& nread, int32_t max = 0);
    Status mark(int32_t readlimit);
    Status reset();
};


#endif
