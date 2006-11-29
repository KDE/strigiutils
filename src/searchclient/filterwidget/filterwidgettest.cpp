#include "filterwidget.h"
#include <QApplication>
using namespace std;

int
main(int argc, char** argv) {
    QApplication qapp(argc, argv);
    FilterWidget fw;
    fw.show();

    QList<QPair<bool,QString> > f;
    f.append(qMakePair(true, QString("*.html")));
    f.append(qMakePair(false, QString(".svn/")));
    fw.setFilters(f);

    return qapp.exec();
}
