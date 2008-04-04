#include "strigidaemonunittestsession.h"
#include <QtCore/QMap>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <iostream>
#include <fstream>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "config.h"
#include "strigilogging.h"

using namespace std;

/**
 * Retrieve the environment settings as a QMap<QString, QString>.
 **/
QMap<QString, QString>
getEnvironment() {
    QMap<QString, QString> env;
    foreach (const QString& val, QProcess::systemEnvironment()) {
        int p = val.indexOf('=');
        if (p > 0) {
            env[val.left(p).toUpper()] = val.mid(p+1);
        }
    }
    return env;
}
/**
 * Unset all environment variables except HOME.
 **/
void
clearEnvironment() {
    QMap<QString, QString> environment = getEnvironment();
    foreach (const QString& s, environment.keys()) {
        if (s != "HOME") {
            unsetenv(s.toAscii());
        }
    }
}
/**
 * Parse the output from the dbus-launch invocation and set the DBUS
 * environment variable in the environment of the current application.
 **/
int
addDBusToEnvironment(QIODevice& io) {
    QByteArray data = io.readLine();
    int pid = -1;
    while (data.size()) {
        if (data[data.size()-1] == '\n') {
            data.resize(data.size()-1);
        }
        QString val(data);
        int p = val.indexOf('=');
        if (p > 0) {
            QString name = val.left(p).toUpper();
            val = val.mid(p+1);
            if (name == "DBUS_SESSION_BUS_PID") {
                pid = val.toInt();
                setenv(name.toAscii(), val.toAscii(), 1);
            } else if (name == "DBUS_SESSION_BUS_ADDRESS") {
                setenv(name.toAscii(), val.toAscii(), 1);
            }
        }
        data = io.readLine();
    }
    return pid;
}
int
startDBusDaemon() {
    // start the dbus process
    QProcess dbusprocess;
    //dbusprocess.setEnvironment(env);
    QStringList dbusargs;
    dbusprocess.start("/usr/bin/dbus-launch", dbusargs, QIODevice::ReadOnly);
    bool ok = dbusprocess.waitForStarted() && dbusprocess.waitForFinished();
    if (!ok) {
        qDebug() << "error starting dbus-launch";
        dbusprocess.kill();
        return -1;
    }

    // add the dbus settings to the environment
    int dbuspid = addDBusToEnvironment(dbusprocess);
    return dbuspid;
}
void
stopDBusDaemon(int dbuspid) {
    // stop the dbus-daemon nicely
    if (dbuspid) kill(dbuspid, 15);
    sleep(1);
    // stop the dbus-daemon harsly (if it is still running)
    if (dbuspid) kill(dbuspid, 9);
    qDebug() << "killing " << dbuspid;
}
void
removeFile(QDir& dir, const QFileInfo& info) {
    if (info.isDir()) {
        QDir d(info.absoluteFilePath());
        foreach(const QFileInfo& i, d.entryInfoList(
                QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot)) {
            removeFile(d, i);
        }
        if (!dir.rmdir(info.fileName())) {
            qDebug() << "could not remove dir " << info.fileName();
        }
    } else {
        if (!dir.remove(info.fileName())) {
            qDebug() << "could not remove " << info.fileName();
        }
    }
}
void
removeDir(const QString& path) {
    QFileInfo i(path);
    QDir parent(i.dir());
    removeFile(parent, i);
}
class StrigiDaemonUnitTestSession::Private {
public:
    string strigidaemon;
    QCoreApplication* app;
    QProcess* strigiDaemonProcess;
    string xdgdir;
    set<string> plugindirs;
    set<string> backends;
    string testdatadir;
    string strigitestdir;
    int dbuspid;

    void setupTestDir();
    QProcess* startStrigiDaemon(const char* daemon);
};
void
StrigiDaemonUnitTestSession::Private::setupTestDir() {
    removeDir(strigitestdir.c_str());
    if (mkdir(strigitestdir.c_str(), 0700) != 0) {
        // abort if we cannot create a new config dir
        cerr << "cannot create directory " << strigitestdir.c_str() << endl;
        exit(1);
    }
    removeDir(testdatadir.c_str());
    if (mkdir(testdatadir.c_str(), 0700) != 0) {
        // abort if we cannot create a new data dir
        cerr << "cannot create directory " << testdatadir.c_str() << endl;
        exit(1);
    }
    // write a config file into the dir
    string file(strigitestdir + "/daemon.conf");
    ofstream out(file.c_str());
    out << "<strigiDaemonConfiguration useDBus='1'>" << endl;
    out << " <repository name='localhost' indexdir='" << strigitestdir
        << "/" << *backends.begin() << "' writeable='1' type='"
        << *backends.begin() << "' "
        << "pollingInterval='0'>";
    out << "  <path path='" + testdatadir + "'/>" << endl;
    out << " </repository>" << endl;
    out << "</strigiDaemonConfiguration>" << endl;
    out.close();
}
QProcess*
StrigiDaemonUnitTestSession::Private::startStrigiDaemon(const char* daemon) {
    QString strigiDaemon = daemon;
    setupTestDir();

    QProcess* strigiDaemonProcess = new QProcess();
    QStringList args;
    args << "-d" << strigitestdir.c_str();
    strigiDaemonProcess->start(strigiDaemon, args, QIODevice::ReadOnly);
    bool ok = strigiDaemonProcess->waitForStarted();
    if (!ok) {
        qDebug() << "could not start " << strigiDaemon;
    }

    return strigiDaemonProcess;
}
void
stopStrigiDaemon(QProcess* strigiDaemonProcess) {
    strigiDaemonProcess->terminate();
    if (!strigiDaemonProcess->waitForFinished(5000)) {
        qDebug() << "Problem finishing process.";
    }
    qDebug() << strigiDaemonProcess->readAllStandardError();
    qDebug() << strigiDaemonProcess->readAllStandardOutput();
    strigiDaemonProcess->close();
    delete strigiDaemonProcess;
    //removeDir(strigitestdir.c_str());
    //removeDir(testdatadir.c_str());
}

StrigiDaemonUnitTestSession::StrigiDaemonUnitTestSession() :p(new Private()) {
    p->app = 0;
}
int argc = 0;
void
StrigiDaemonUnitTestSession::start() {
    // unset all environment variables except HOME
    clearEnvironment();
    p->app = new QCoreApplication(argc, 0);//argc, argv);

    // start the required daemons and wait for them to start up
    p->dbuspid = startDBusDaemon();
    // set some environment variables so that strigi can find the desired
    // files from the source and build directories
    // This ensures we test the development version, not the installed version
    setenv("XDG_DATA_HOME", p->xdgdir.c_str(), 1);
    setenv("XDG_DATA_DIRS", p->xdgdir.c_str(), 1);
    string plugindir;
    for (set<string>::const_iterator i = p->plugindirs.begin();
            i != p->plugindirs.end(); ++i) {
        if (i != p->plugindirs.begin()) {
            plugindir += PATH_SEPARATOR;
        }
        plugindir += *i;
    }
    setenv("STRIGI_PLUGIN_PATH", plugindir.c_str(), 1);

    p->strigiDaemonProcess = p->startStrigiDaemon(p->strigidaemon.c_str());

    STRIGI_LOG_INIT_BASIC()
}

StrigiDaemonUnitTestSession::~StrigiDaemonUnitTestSession() {
    delete p->app;
    delete p;
}
void
StrigiDaemonUnitTestSession::stop() {
    stopStrigiDaemon(p->strigiDaemonProcess);
    stopDBusDaemon(p->dbuspid);
}
void
StrigiDaemonUnitTestSession::setStrigiDaemon(const char* strigidaemon) {
    p->strigidaemon = strigidaemon;
}
void
StrigiDaemonUnitTestSession::setXDGDir(const char* xdgdir) {
    p->xdgdir = xdgdir;
}
void
StrigiDaemonUnitTestSession::addBackend(const char* name, const char* plugindir){
    p->backends.insert(name);
    p->plugindirs.insert(plugindir);
}
void
StrigiDaemonUnitTestSession::addFile(const char* name, const char* content, 
        int contentlength) {
    string path = p->testdatadir + '/' + name;
    FILE* f = fopen(path.c_str(), "w");
    fwrite(content, 1, contentlength, f);
    fclose(f);
}
void
StrigiDaemonUnitTestSession::setStrigiDir(const char* dir) {
    p->strigitestdir = dir;
}
void
StrigiDaemonUnitTestSession::setDataDir(const char* dir) {
    p->testdatadir = dir;
}

