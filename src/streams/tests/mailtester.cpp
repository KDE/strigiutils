#include "../mailinputstream.h"
#include "../fileinputstream.h"
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <string>
using namespace jstreams;

void
testMailFile2(StreamBase<char>* input) {
    MailInputStream mail(input);
    StreamBase<char>* a = mail.nextEntry();
    while (a) {
        bool open = false; 
        std::string filename = "/tmp/mailtest/"+mail.getEntryInfo().filename;
        QFile file(filename.c_str());
        if (mail.getEntryInfo().filename.length() > 0) {
            open = file.open(QIODevice::WriteOnly|QIODevice::Truncate);
        }
        int32_t size = 0;
        bool readall = true;
        const char* start;
        int32_t nread = a->read(start, 1024, 0);
        //printf("# %i\n", nread);
        while (readall && nread > 0) {
            if (open) {
                //printf("writing: %i\t%i\t%s\n", nread, size, filename.c_str());
                file.write(start, nread);
            }
            size += nread;
            nread = a->read(start, 1, 0);
        }
        //if (size == 0) { a = 0; a->read(start, 1,1);}
        //printf(": %p\t%i\t%i\n", a, size, nread);
        if (open) {
            file.close();
        }
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
    int32_t nread = file.read(header, 1024, 1024);
    bool ok = nread > 0 && MailInputStream::checkHeader(header, nread);
    if (ok) {
        file.reset(0);
        printf("mail %s\n", (const char*)filename);
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
main(int argc, char **argv) {
    QList<QByteArray> mailfiles;
    if (argc < 2) {
        const char* dir = "/.kde/share/apps/kmail/mail";
        mailfiles = getMailFiles(QDir::homePath()+dir);
    } else {
        for (int i=1; i<argc; ++i) {
            mailfiles += QByteArray(argv[i]);
        }
    }
    foreach (QByteArray mailfile, mailfiles) {
        testMailFile(mailfile);
    }
    return 0;
}
