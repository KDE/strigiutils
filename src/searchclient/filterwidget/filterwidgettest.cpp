#include "filterwidget.h"
#include <QApplication>
using namespace std;

int
main(int argc, char** argv) {
    QApplication qapp(argc, argv);
    FilterWidget fw;
    fw.show();

    vector<pair<bool,string> > f;
    f.push_back(make_pair(true, "*.html"));
    f.push_back(make_pair(false, ".svn/"));
    fw.setFilters(f);

    return qapp.exec();
}
