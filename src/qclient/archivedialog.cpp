#include "archiveenginehandler.h"

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>

/**
 * Very simple example that allows the user to open files
 * in archives. Archive files can be opened as directories.
 **/
int
main(int argc, char **argv) {
	QApplication app(argc, argv);

	ArchiveEngineHandler engine;
	QFileDialog dialog;
	dialog.show();

	return app.exec();
}

