set(DIRS filters streams streamindexer dummyindexer archivereader indexertests
    xmlindexer combinedindexer)

if (CLucene_FOUND)
  set(DIRS ${DIRS} luceneindexer)
endif (CLucene_FOUND)

if (HyperEstraier_FOUND)
  set(DIRS ${DIRS} estraierindexer)
endif (HyperEstraier_FOUND)

if (SQLite_FOUND)
  set(DIRS ${DIRS} sqliteindexer)
endif (SQLite_FOUND)

set(DIRS ${DIRS} daemon htmlgui searchclient)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
