#include <QtCore/QAbstractFileEngineHandler>
#include <QtCore/QDateTime>
#include "inputstream.h"
#include "archiveengine.h"

class StreamEngine : public QAbstractFileEngine {
private:
    size_t pos_;
    InputStream *stream;
    const FileEntry* entry;
    ArchiveEngineBase *archive;

public:
    StreamEngine(const FileEntry* entry, ArchiveEngineBase* engine);
    ~StreamEngine();
    InputStream *getInputStream();
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        return type & QAbstractFileEngine::FileType;
    }
    QString fileName ( FileName file = DefaultName ) const;
    bool open(QIODevice::OpenMode mode);
    qint64 read(char* data, qint64 maxlen);
//    qint64 readLine(char *data, qint64 maxlen);
    qint64 size () const;
    QDateTime fileTime ( FileTime time ) const {
        if (time == ModificationTime) {
            return entry->mtime;
        } else if (time == AccessTime) {
            return entry->atime;
        }
        return entry->ctime;
    }
    const FileEntry* getFileEntry() const {
        return entry;
    }
};
