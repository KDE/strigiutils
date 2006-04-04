#ifndef ARCHIVEDIRENGINE
#define ARCHIVEDIRENGINE

#include "archiveengine.h"

class ArchiveDirEngine : public ArchiveEngineBase {
private:
    ArchiveEngineBase* parent;
    const FileEntry* const entry;
protected:
    bool nextEntry() const {
        return parent->nextEntry();
    }
    void readEntryNames() const {
        parent->readEntryNames();
    }
public:
    ArchiveDirEngine(ArchiveEngineBase* parent,
        const FileEntry*e);
    ~ArchiveDirEngine();
    StreamEngine *openEntry(const QString &filename);
    ArchiveDirEngine *openDir(QString filename);
    QString fileName ( FileName file = DefaultName ) const;
    QStringList entryList(QDir::Filters filters,
        const QStringList& filterNames) const;
    InputStream *getInputStream(const FileEntry* entry);
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        // signal that this is file _and_ a "directory"
        FileFlags flags =
            QAbstractFileEngine::DirectoryType;
        return flags & type;
    }
};

#endif
