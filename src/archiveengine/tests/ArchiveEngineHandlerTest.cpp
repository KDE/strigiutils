#include "ArchiveEngineHandlerTest"
#include "archiveenginehandler.h"
#include <QtTest/QtTest>
#include <QDebug>
void
ArchiveEngineHandlerTest::testFile() {
    ArchiveEngineHandler engine;
    QFileInfo f("data/a.zip/file1");
    QVERIFY(f.exists());
}

