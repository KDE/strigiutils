#include "archiveenginehandler.h"
#include "archiveengine.h"
#include "archivedirengine.h"
#include "streamengine.h"
#include <QtCore/QFSFileEngine>
#include <dirent.h>

bool
ArchiveEngineHandler::open(StreamEngine** se, ArchiveEngineBase** ae, const QString& name) const {
//    printf("open %p %p '%s'\n", *se, *ae, (const char*)name.toUtf8());
    bool newstream = *se == 0;
    if (*ae) {
        *se = (*ae)->openEntry(name);
        if (*se == 0) {
            ArchiveEngineBase *lae = (*ae)->openDir(name);
            //*ade = (*ae)->openDir(name);
            if (lae == 0) {
                delete *ae;
                *ae = 0;
                return false;
            }
            *ae = lae;
            return true;
        }
    }

    // we now have a stream in *sa

    // is the current stream an archive?
    *ae = new ArchiveEngine(*se);

    if ((*ae)->error() == QFile::NoError) {
        *se = 0;
        return true;
    }

    // release engine for to be deleted object,
    // so that we can continue using this stream
    (*ae)->releaseEngines();
    delete *ae;
    *ae = 0;
    if (!newstream) {
        delete *se;
        *se = 0;
        return false;
    }

    return true;
}
QAbstractFileEngine *
ArchiveEngineHandler::create(const QString &file) const {

    // try to open the deepest regular file in the file path
    QFSFileEngine* fse = 0;
    // use the full path
    QString path = file;
    do {
        fse = new QFSFileEngine(path);
        if (fse->open(QIODevice::ReadOnly)) {
            // make sure this is not a directory
            // cannot use Qt functions because of bugs
            // (bug is fixed in Qt 4.1.2
#if QT_VERSION >= 0x040102
            if (fse->fileFlags(QAbstractFileEngine::DirectoryType)) {
#else
            DIR* dir = opendir(path.toUtf8());
            if (dir) {
                closedir(dir);
#endif
                delete fse;
                return 0;
            }
        } else {
            delete fse;
            fse = 0;

            // strip off the last part of the path
            int pos = path.lastIndexOf('/');
            if (pos == -1) {
                path = "";
            } else {
                path = path.left(pos);
            }
        }
    } while (fse == 0 && !path.isEmpty());
    if (fse == 0) {
        // no file could be opened
        return 0;
    }

    // try to open the file as an archive
    //FSFileInputStream *ffis = new FSFileInputStream(fse);
    ArchiveEngineBase *ae = 0;
    ae = new ArchiveEngine(path, fse);
    if (ae->error() != QFile::NoError) {
        delete ae;
        return 0;
    }

    StreamEngine *se = 0;
    int len = path.length();
    int pos = file.indexOf('/', len+1);
    QString name;
    while (pos != -1) {
        name = file.mid(len+1, pos-len-1);
        if (!open(&se, &ae, name)) {
            return 0;
        }

        path += '/'+name;
        len = pos;
        pos = file.indexOf('/', len+1);
    }
    if (file.length() > len+1) {
        name = file.mid(len+1);
 //      printf(" %s %s\n", (const char*)path.toUtf8(), (const char*)name.toUtf8());
        if (!open(&se, &ae, name)) {
            return 0;
        }
    }

    // return the stream that is not 0
    if (se) {
        return se;
    }
    return ae;
}
