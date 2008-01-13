/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <stdexcept>

#include "strigilogging.h"
#include "config.h"
#include "config.h"
#include "strigiclient.h"
#include <signal.h>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
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
const QString teststrigidir = "teststrigidir";
QProcess*
startStrigiDaemon() {
    QString strigiDaemon = BINARYDIR"/src/daemon/strigidaemon";

    QProcess* strigiDaemonProcess = new QProcess();
    QStringList args;
    args << "-d" << teststrigidir;
    strigiDaemonProcess->start(strigiDaemon, args, QIODevice::ReadOnly);
    bool ok = strigiDaemonProcess->waitForStarted();
    if (!ok) {
        qDebug() << "could not start " << strigiDaemon;
    }

    return strigiDaemonProcess;
}
void
removeFile(QDir& dir, const QFileInfo& info) {
    if (info.isDir()) {
        QDir d(info.absoluteFilePath());
        foreach(const QFileInfo& i, d.entryInfoList(QDir::AllEntries | QDir::System
                                                    | QDir::NoDotAndDotDot)) {
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
    removeDir(teststrigidir);
}
int
doTests() {
    // Get the top level suite from the registry
    CppUnit::Test *suite;
    suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextTestRunner runner;
    runner.addTest( suite );
    
    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener( &result );

    // Add a listener that print dots as test run.
    CppUnit::TextTestProgressListener progress;
    controller.addListener( &progress );

    try {
        runner.run( controller);

        std::cerr << std::endl;

     // Print test in a compiler compatible format.
        CppUnit::CompilerOutputter outputter( &result, std::cerr );
        outputter.write();
    } catch ( std::invalid_argument &e )  // Test path not resolved
    {
        std::cerr  <<  std::endl
                <<  "ERROR: "  <<  e.what()
                << std::endl;
        return 0;
    }
    return result.wasSuccessful() ? 0 : 1;
}
int
main(int argc, char** argv) {
    // unset all environment variables except HOME
    clearEnvironment();

    // start the required daemons and wait for them to start up
    int dbuspid = startDBusDaemon();
    // set some environment variables so that strigi can find the desired
    // files from the source and build directories
    // This ensures we test the development version, not the installed version
    setenv("XDG_DATA_HOME", SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("XDG_DATA_DIRS", SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("STRIGI_PLUGIN_PATH", BINARYDIR"/src/luceneindexer/:"
        BINARYDIR"/src/estraierindexer:"BINARYDIR"/src/sqliteindexer", 1);
    QProcess* strigiDaemonProcess = startStrigiDaemon();

    STRIGI_LOG_INIT_BASIC()

    int result = doTests();

    // stop the daemons
    stopStrigiDaemon(strigiDaemonProcess);
    stopDBusDaemon(dbuspid);

    return result;
}
