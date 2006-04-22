#ifndef ARCHIVEENGINEHANDLERTEST_H
#define ARCHIVEENGINEHANDLERTEST_H

#include <QObject>

class ArchiveEngineHandlerTest : public QObject {
Q_OBJECT
public:
    ~ArchiveEngineHandlerTest() {}
private slots:
    void testFile();
};

#endif
