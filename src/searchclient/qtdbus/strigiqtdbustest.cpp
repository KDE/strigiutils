#include "strigiclient.h"
#include <QCoreApplication>
#include <QDebug>

void
printStrigiStatus(StrigiClient& strigi) {
    // query strigi
    QMap<QString,QString> r = strigi.getStatus();
    QTextStream out(stdout);
    QMapIterator<QString, QString> i(r);
    while (i.hasNext()) {
        i.next();
        out << i.key() << ": " << i.value() << endl;
    }
}
void
printStrigiQuery(StrigiClient& strigi, const QString& query) {
    // query strigi
    QList<StrigiHit> r = strigi.getHits(query, 10, 0);
    QTextStream out(stdout);
    QList<StrigiHit> h = r;

    // print the paths for the first 10 hits
    foreach (const StrigiHit& sh, h) {
        out << sh.uri << endl;
    }
}
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    // initialize the interface to Strigi
    StrigiClient strigi;

    // get strigis status
    printStrigiStatus(strigi);

    // do a query
    printStrigiQuery(strigi, "kde");

    return 0;
}
