#ifndef ARCHIVEENGINE
#define ARCHIVEENGINE

#include <QtCore/QAbstractFileEngineHandler>

class FileEntry;
class FileEntry {
private:
    QList<FileEntry*> entries;
public:
    QString name;
    qint64 size;
    QAbstractFileEngine::FileFlags fileFlags;

    FileEntry(const QString &n):name(n){}
    ~FileEntry();
    FileEntry* add(const QString &name);
    FileEntry* getEntry(const QString &name);
    const FileEntry* getEntry(const QString &name) const;
    const QList<FileEntry*> getEntries() const {
        return entries;
    }
};

class StreamEngine;
class ArchiveDirEngine;
class InputStream;

class ArchiveEngineBase : public QAbstractFileEngine {
friend class ArchiveDirEngine;
private:
    virtual bool nextEntry() const= 0;
    virtual void readEntryNames() const = 0;
public:
    virtual StreamEngine *openEntry(const QString &filename) = 0;
    virtual ArchiveDirEngine *openDir(QString filename) = 0;
    virtual InputStream *getInputStream(const FileEntry* entry) = 0;
    // lose references to engines so that they are not deleted
    virtual void releaseEngines() {};
    bool caseSensitive () const { return true; }
    bool close () { return true; }
    bool copy ( const QString & newName ) { return false; }
    bool isRelativePath () const {
        return false;
    }
    bool isSequential () const {
        return true;
    }
    bool open ( QIODevice::OpenMode mode ) {
        printf("open archive");
        return false;
    }
};

class InputStream;
class SubInputStream;
class FSFileInputStream;
class SubStreamProvider;

class ArchiveEngine : public ArchiveEngineBase {
private:
    QString path;
    StreamEngine *streamengine;
    InputStream *parentstream;
    FSFileInputStream *filestream;
    SubStreamProvider *zipstream;
    mutable SubInputStream *entrystream;
    bool readAllEntryNames;
    mutable FileEntry entry;
    mutable FileEntry* current;

    bool nextEntry() const;
    void reopen();
    void readEntryNames() const;
protected:
    const QLinkedList<FileEntry>* getEntries(const QString& base);
public:
    ArchiveEngine(const QString& path, FSFileInputStream *fs);
    ArchiveEngine(StreamEngine *fs);
    ~ArchiveEngine();
    StreamEngine *openEntry(const QString &filename);
    ArchiveDirEngine *openDir(QString filename);
    QStringList entryList(QDir::Filters filters,
        const QStringList& filterNames) const;
    QString fileName ( FileName file = DefaultName ) const;
    // lose references to engines so that they are not deleted
    void releaseEngines() {    streamengine = 0;};
    InputStream *getInputStream(const FileEntry* entry);
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        // signal that this is file _and_ a "directory"
        FileFlags flags =
            QAbstractFileEngine::DirectoryType|QAbstractFileEngine::FileType;
        return flags & type;
    }
};

#endif
