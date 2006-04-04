#include "archiveengine.h"
#include "archivedirengine.h"
#include "fsfileinputstream.h"
#include "zipinputstream.h"
#include "streamengine.h"
#include "filebrowser.h"

#include <QtGui/QApplication>
#include <QtGui/QTreeView>
#include <QtGui/QDirModel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <Qt/QFSFileEngine>
#include <sys/types.h>
#include <dirent.h>

class ArchiveEngineHandler : public QAbstractFileEngineHandler {
private:
    bool open(StreamEngine** se, ArchiveEngineBase** ae, const QString& name) const;
public:
	QAbstractFileEngine *create(const QString &fileName) const;
};
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

   // if (file.contains(".zip")||file.contains(".jar"))
    //printf("opening file '%s'\n", (const char*)file.toUtf8());

    // try to open a normal file
    QFSFileEngine* fse = 0;
    QString path = file;
    do {
        fse = new QFSFileEngine(path);
        if (fse->open(QIODevice::ReadOnly)) {
            // make sure this is not a directory
            // cannot use Qt functions because of bugs
            DIR* dir = opendir(path.toUtf8());
            if (dir) {
                closedir(dir);
                delete fse;
                return 0;
            }
        } else {
            delete fse;
            fse = 0;
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

    FSFileInputStream *ffis = new FSFileInputStream(fse);
    ArchiveEngineBase *ae = 0;
    ae = new ArchiveEngine(path, ffis);
    if (ae->error() != QFile::NoError) {
        delete ae;
        return 0;
    }

    // try to open an archive, a compressed stream or an archive
    // directory here
    StreamEngine *se = 0;

    int len = path.length();
    int pos = file.indexOf('/', len+1);
    QString name;
    while (pos != -1) {
        name = file.mid(len+1, pos-len-1);
        if (!open(&se, &ae, name)) return 0;

//      printf(" %s %s\n", (const char*)path.toUtf8(), (const char*)name.toUtf8());

        path += '/'+name;
        len = pos;
        pos = file.indexOf('/', len+1);
    }
    if (file.length() > len+1) {
       name = file.mid(len+1);
 //      printf(" %s %s\n", (const char*)path.toUtf8(), (const char*)name.toUtf8());
       if (!open(&se, &ae, name)) return 0;
    }

    QAbstractFileEngine *afe = 0;
    if (se) afe = se;
    if (ae) afe = ae;
    if (afe) printf("-- opened file %s\n", (const char*)afe->fileName().toUtf8());
    return afe;
}
int
main(int argc, char **argv) {
	QApplication app(argc, argv);

	ArchiveEngineHandler engine;

    if (argc > 1) {
        QFile file(argv[1]);
        if (!file.open(QIODevice::ReadOnly)) {
            printf("Could not open '%s'\n", argv[1]);
            return 1;
        }
        const int bufsize=10;
        char buf[bufsize];
        file.read(buf, bufsize);
        /*QStringList l;
        if (false) {
            l = engine.create(argv[1])->entryList(QDir::AllEntries, l);
        } else {
            QDir i(argv[1]);
            l = i.entryList();
        }
        printf("---\n");
        foreach(QString s, l) {
            printf("%s\n", (const char*)s.toUtf8());
        }
        printf("---\n"); */
        return 0;
    } else {
        FileBrowser *fb = new FileBrowser();
        fb->show();
    	return app.exec();
    }
}

