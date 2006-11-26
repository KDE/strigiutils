set(DIRS filters streams streamindexer dummyindexer archivereader xmlindexer)

if (CLucene_FOUND)
  set(DIRS ${DIRS} luceneindexer)
endif (CLucene_FOUND)

if (HyperEstraier_FOUND)
  set(DIRS ${DIRS} estraierindexer)
endif (HyperEstraier_FOUND)

if (SQLite_FOUND)
  set(DIRS ${DIRS} sqliteindexer)
endif (SQLite_FOUND)

if (Lucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)
  set(DIRS ${DIRS} indexertests combinedindexer daemon htmlgui searchclient)
endif (Lucene_FOUND OR HyperEstraier_FOUND OR SQLite_FOUND)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
