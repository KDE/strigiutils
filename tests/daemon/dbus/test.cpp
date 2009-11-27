#include "config.h"
#include "strigiclient.h"
#include <signal.h>
#include <sys/stat.h>
#include <fstream>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
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
    QStringList dbusargs;
    dbusprocess.start(QString::fromUtf8(DBUSLAUNCH_EXECUTABLE), dbusargs, QIODevice::ReadOnly);
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
std::string strigitestdir = "strigitestdir";
std::string backend = "clucene";
std::string testdatadir = "testdatadir";
void
setupTestDir() {
    if (mkdir(strigitestdir.c_str(), 0700) != 0) {
        // abort if we cannot create a new config dir
        exit(1);
    }
    // write a config file into the dir
    string file(strigitestdir + "/daemon.conf");
    ofstream out(file.c_str());
    out << "<strigiDaemonConfiguration useDBus='1'>" << endl;
    out << " <repository name='localhost' indexdir='" + strigitestdir
        + "/" + backend + "' type='" + backend + "'>" << endl;
    out << "  <path path='" + testdatadir + "'/>" << endl;
    out << " </repository>" << endl;
    out << "</strigiDaemonConfiguration>" << endl;
    out.close();
}
QProcess*
startStrigiDaemon() {
    QString strigiDaemon = BINARYDIR"/src/daemon/strigidaemon";

    setupTestDir();

    QProcess* strigiDaemonProcess = new QProcess();
    QStringList args;
    args << "-d" << strigitestdir.c_str();
    strigiDaemonProcess->start(strigiDaemon, args, QIODevice::ReadOnly);
    bool ok = strigiDaemonProcess->waitForStarted();
    if (!ok) {
        qDebug() << "could not start " << strigiDaemon;
    }
    unlink(strigitestdir.c_str());

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
}

void
doTests() {
    StrigiClient strigiclient;
    qDebug() << strigiclient.getStatus();
}
int
main() {
    // unset all environment variables except HOME
    clearEnvironment();

    // start the required daemons and wait for them to start up
    int dbuspid = startDBusDaemon();
    // set some environment variables so that strigi can find the desired
    // files from the source and build directories
    // This ensures we test the development version, not the installed version
    setenv("XDG_DATA_HOME", SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("XDG_DATA_DIRS", SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("STRIGI_PLUGIN_PATH", BINARYDIR"/src/luceneindexer/" PATH_SEPARATOR
        BINARYDIR"/src/estraierindexer" PATH_SEPARATOR
        BINARYDIR"/src/sqliteindexer", 1);
    QProcess* strigiDaemonProcess = startStrigiDaemon();
    sleep(1);

    doTests();

    // stop the daemons
    stopStrigiDaemon(strigiDaemonProcess);
    stopDBusDaemon(dbuspid);
    return 0;
}
