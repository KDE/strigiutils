# On Solaris, we need -lsocket and -lnsl for networking in libraries;
# Check copied from kdelibs FindX11.cmake.
CHECK_LIBRARY_EXISTS("socket" "connect" "" CMAKE_LIB_SOCKET_HAS_CONNECT)

set(DIRS streams streamanalyzer archivereader combinedindexer dummyindexer)

if (BUILD_UTILS)
    add_subdirectory(strigicmd)
    add_subdirectory(xmlindexer)
endif (BUILD_UTILS)

if (CppUnit_FOUND)
    add_subdirectory(indexertests)
endif (CppUnit_FOUND)

if (CLucene_FOUND)
  set(DIRS ${DIRS} luceneindexer)
endif (CLucene_FOUND)

if (HyperEstraier_FOUND)
  set(DIRS ${DIRS} estraierindexer)
  message("** HyperEstraier support is broken. Do not rely on it. **")
endif (HyperEstraier_FOUND)

if (SQLite_FOUND)
  set(DIRS ${DIRS} sqliteindexer)
  message("** SQLite support is broken. Do not rely on it. **")
endif (SQLite_FOUND)

if(NOT WIN32)
  set(DIRS ${DIRS} htmlgui)
endif(NOT WIN32)

if (QT_QTDBUS_FOUND)
  set(DIRS ${DIRS} searchclient)
endif (QT_QTDBUS_FOUND)

# make sure we build the daemon on all architectures - not as long as sys/socket.h is used
if(BUILD_DAEMON AND NOT WIN32)
  add_subdirectory(daemon)
  add_subdirectory(xsd)
endif(BUILD_DAEMON AND NOT WIN32)

if (NOT CLucene_FOUND)
  message("** No CLucene libraries were found, so Strigi cannot use indexes.")
  message("** It is required to install CLucene >= ${CLUCENE_MIN_VERSION}.")
  message("** You will still be able to use deepfind, deepgrep and xmlindexer.")
endif (NOT CLucene_FOUND)

if (QT4_FOUND)
  set(DIRS ${DIRS} archiveengine qclient)
endif (QT4_FOUND)

SUBDIRS(${DIRS})
