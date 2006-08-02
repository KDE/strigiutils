set(DIRS streams streamindexer dummyindexer)

if (CLucene_FOUND)
  set(DIRS ${DIRS} luceneindexer)
endif (CLucene_FOUND)

if (HyperEstraier_FOUND)
  set(DIRS ${DIRS} estraierindexer)
endif (HyperEstraier_FOUND)

set(DIRS ${DIRS} daemon htmlgui searchclient eventlistener)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
