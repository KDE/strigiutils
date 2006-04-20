#ifndef FSFILEINPUTSTREAM_H
#define FSFILEINPUTSTREAM_H

#include "bufferedstream.h"

class QFSFileEngine;
class QString;

namespace jstreams {

class FSFileInputStream : public BufferedInputStream<char> {
private:
    bool open;
    QFSFileEngine *fse;

    void readFromFile();
protected:
    bool fillBuffer();
public:
    static const int32_t defaultBufferSize;
    FSFileInputStream(const QString &filename, int32_t buffersize=defaultBufferSize);
    FSFileInputStream(QFSFileEngine *, int32_t buffersize=defaultBufferSize);
    ~FSFileInputStream();
    StreamStatus reopen();
};

} // end namespace jstreams

#endif
