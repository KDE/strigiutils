#include "archiveenginehandler.h"
#include "filebrowser.h"

#include <QtGui/QApplication>

int
main(int argc, char **argv) {
	QApplication app(argc, argv);

	ArchiveEngineHandler engine;

    if (argc > 1) {
        QFile file(argv[1]);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("Could not open '%s'\n", argv[1]);
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
        FileBrowser fb;
        fb.show();
    	return app.exec();
    }
}

