set(DIRS streams streamindexer dummyindexer)

if (FOUND_CLUCENE)
  set(DIRS ${DIRS} luceneindexer)
endif (FOUND_CLUCENE)

set(DIRS ${DIRS} daemon htmlgui searchclient)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
