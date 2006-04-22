#include "ArchiveEngineHandlerTest.h"
#include "archiveenginehandler.h"
#include <QtTest/QtTest>
#include <QDebug>
void
ArchiveEngineHandlerTest::testFile() {
    ArchiveEngineHandler engine;
    QFileInfo f1("a.zip");
    QVERIFY(f1.exists());
    QFileInfo f2("a.zip/file1");
    QVERIFY(f2.exists());
}

