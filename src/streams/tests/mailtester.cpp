#include "../mailinputstream.h"
#include "../fileinputstream.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <string>
using namespace jstreams;

void
testMailFile2(StreamBase<char>* input) {
    MailInputStream mail(input);
    StreamBase<char>* a = mail.nextEntry();
    while (a) {
        int32_t size = 0;
        const char* start;
        int32_t nread = a->read(start);
        while (nread > 0) {
            size += nread;
            nread = a->read(start);
        }
        printf("%p\t%i\n", a, size);
        a = mail.nextEntry();
    }
//    qDebug() << mail.getSubject().c_str();
//    qDebug() << mail.getContentType().c_str();
}

void
testMailFile(const QByteArray &filename) {
    FileInputStream file((const char*)filename);
    file.mark(10000);
    const char* header;
    file.read(header, 50);
    bool ok = MailInputStream::checkHeader(header, 50);
    if (ok) {
        testMailFile2(&file);
    }
}

QList<QByteArray>
getMailFiles(const QString& dir) {
    QList<QByteArray> files;
    QProcess find;
    find.start("find", QStringList() << dir << "-type" << "f");
    if (!find.waitForStarted()) {
        qDebug() << "Could not run 'find'";
        return files;
    }
    find.closeWriteChannel();

    if (!find.waitForFinished()) {
        qDebug() << "Could not run 'find'";
        return files;
    }
    if (find.exitCode()) {
        qDebug() << find.readAllStandardError();
        return files;
    }
    QByteArray line = find.readLine();
    while (!line.isEmpty()) {
        line.chop(1);
        files.append(line);
        line = find.readLine();
    }
    return files;
}

int
main() {
    const char* dir = "/.kde/share/apps/kmail/mail";
    QList<QByteArray> mailfiles = getMailFiles(QDir::homePath()+dir);
    foreach (QByteArray mailfile, mailfiles) {
        qDebug() << mailfile;
        testMailFile(mailfile);
    }
    return 0;
}
