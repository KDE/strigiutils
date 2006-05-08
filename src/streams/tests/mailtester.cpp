#include "../mailinputstream.h"
#include "../fileinputstream.h"
#include <QDebug>
#include <QDir>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using namespace jstreams;

void
testMailFile2(StreamBase<char>* input) {
    MailInputStream mail(input);
    StreamBase<char>* a = mail.nextEntry();
    while (a) {
        std::string filename = "/tmp/mailtest/"+mail.getEntryInfo().filename;
        FILE* file = 0;
        if (mail.getEntryInfo().filename.length() > 0) {
            file = fopen(filename.c_str(), "w");
        }
        int32_t size = 0;
        bool readall = true;
        const char* start;
        int32_t nread = a->read(start, 1024, 0);
        //printf("# %i\n", nread);
        while (readall && nread > 0) {
            if (file) {
                //printf("writing: %i\t%i\t%s\n", nread, size, filename.c_str());
                fwrite(start, 1, nread, file);
            }
            size += nread;
            nread = a->read(start, 1, 0);
        }
        //if (size == 0) { a = 0; a->read(start, 1,1);}
        //printf(": %p\t%i\t%i\n", a, size, nread);
        if (file) {
            fclose(file);
        }
        a = mail.nextEntry();
    }
//    qDebug() << mail.getSubject().c_str();
//    qDebug() << mail.getContentType().c_str();
}

void
testMailFile(const char* filename) {
    FileInputStream file(filename);
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

void
testMails(const char* path) {
    struct stat s;
    if (stat(path, &s) != 0) return;
    if (S_ISDIR(s.st_mode)) {
        std::string dir(path);
        if (dir[dir.length()-1] == '.') return;
        dir += "/";
        DIR* d = opendir(dir.c_str());
        if (d == 0) return;
        struct dirent* e = readdir(d);
        while(e) {
            std::string ep = dir+e->d_name;
            testMails(ep.c_str());
            e = readdir(d);
        }
        closedir(d);
    } else if (S_ISREG(s.st_mode)) {
        testMailFile(path);
    }
}

int
main(int argc, char **argv) {
    if (argc < 2) {
        QString dir = QDir::homePath()+"/.kde/share/apps/kmail/mail";
        testMails(dir.toLocal8Bit());
        dir = QDir::homePath()+"/Mail";
        testMails(dir.toLocal8Bit());
    } else {
        for (int i=1; i<argc; ++i) {
            testMails(argv[i]);
        }
    }
    return 0;
}
