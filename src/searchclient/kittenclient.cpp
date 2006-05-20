#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include "simplesearchgui.h"

int
main(int argc, char** argv) {
    QApplication app(argc, argv);
    SimpleSearchGui ssg;
    ssg.show();
    return app.exec();
}

