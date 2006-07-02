set(DIRS streams streamindexer dummyindexer)

if (FOUND_CLUCENE)
    set(DIRS ${DIRS} luceneindexer)
endif (FOUND_CLUCENE)

set(DIRS ${DIRS} daemon archiveengine qclient)

SUBDIRS(${DIRS})
