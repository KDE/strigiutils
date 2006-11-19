#include "strigiasyncwidget.h"
#include <QApplication>
#include <QDebug>

int
main(int argc, char **argv) {
    QApplication app(argc, argv);

    StrigiAsyncWidget strigi;
    strigi.show();
    return app.exec();
}
