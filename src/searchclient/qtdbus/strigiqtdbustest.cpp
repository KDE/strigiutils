#include "strigiclient.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtDBus/qdbusmetatype.h>
#include <QDebug>

void
printStrigiStatus(VandenoeverStrigiInterface& strigi) {
   // query strigi
   QDBusReply<QMap<QString,QString> > r = strigi.getStatus();
   if (r.isValid()) {
       QTextStream out(stdout);
       QMapIterator<QString, QString> i(r);
       while (i.hasNext()) {
           i.next();
           out << i.key() << ": " << i.value() << endl;
       }
   } else {
       QTextStream err(stderr);
       err << r.error().message() << endl;
   }
}
void
printStrigiQuery(VandenoeverStrigiInterface& strigi, const QString& query) {
   // query strigi
   QDBusReply<QList<StrigiHit> > r = strigi.getHits(query, 10, 0);
   if (r.isValid()) {
       QTextStream out(stdout);
       QList<StrigiHit> h = r;

       // print the paths for the first 10 hits
       foreach (StrigiHit sh, h) {
           out << sh.uri << endl;
       }
   } else {
       QTextStream err(stderr);
       err << r.error().message() << endl;
   }
}
int main(int argc, char **argv) {
   QCoreApplication app(argc, argv);

   // register the custom types
   qDBusRegisterMetaType<QMap<QString,QString> >();
   qDBusRegisterMetaType<QMultiMap<int,QString> >();
   qDBusRegisterMetaType<QList<StrigiHit> >();
   qDBusRegisterMetaType<StrigiHit>();

   // initialize the interface to Strigi
   VandenoeverStrigiInterface strigi("vandenoever.strigi", "/search",
       QDBusConnection::sessionBus());

   // get strigis status
   printStrigiStatus(strigi);

   // do a query
   printStrigiQuery(strigi, "kde");

   return 0;
}
