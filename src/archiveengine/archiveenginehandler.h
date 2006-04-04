#ifndef ARCHIVEENGINEHANDLER_H
#define ARCHIVEENGINEHANDLER_H

#include <QtCore/QAbstractFileEngineHandler>

class StreamEngine;
class ArchiveEngineBase;

class ArchiveEngineHandler : public QAbstractFileEngineHandler {
private:
    bool open(StreamEngine** se, ArchiveEngineBase** ae, const QString& name) const;
public:
    QAbstractFileEngine *create(const QString &fileName) const;
};

#endif
