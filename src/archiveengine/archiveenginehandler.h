#ifndef ARCHIVEENGINEHANDLER_H
#define ARCHIVEENGINEHANDLER_H

#include <QtCore/QAbstractFileEngineHandler>

class StreamEngine;
class ArchiveEngineBase;

/**
 * @short Class to register the custom file engine ArchiveEngine.
 *
 * This class is an implementation of QAbstractFileEngine.
 */
class ArchiveEngineHandler : public QAbstractFileEngineHandler {
private:
    bool open(StreamEngine** se, ArchiveEngineBase** ae, const QString& name) const;
public:
    QAbstractFileEngine *create(const QString &fileName) const;
};

#endif
