set(DIRS streams streamindexer dummyindexer)

if (CLUCENE_FOUND)
  set(DIRS ${DIRS} luceneindexer)
endif (CLUCENE_FOUND)

set(DIRS ${DIRS} daemon htmlgui searchclient)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
