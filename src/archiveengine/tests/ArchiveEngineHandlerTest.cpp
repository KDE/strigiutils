#include "ArchiveEngineHandlerTest"
#include "archiveenginehandler.h"
#include <QtTest/QtTest>

void
ArchiveEngineHandlerTest::testFile() {
    qDebug("hmm");
    ArchiveEngineHandler engine;
    QFile f("hmm");
    QVERIFY(f.exists());
}

