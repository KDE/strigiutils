# xsd has been removed until the next release
set(DIRS eventlistener)

IF(ENABLE_DBUS)
  set(DIRS ${DIRS} dbus)
ENDIF(ENABLE_DBUS)

subdirs(${DIRS})

include_directories( ../streamindexer ../streams xsd dbus)

IF(ENABLE_INOTIFY)
  add_definitions(-DHAVE_INOTIFY)
  MESSAGE("-- inotify support enabled")
ENDIF(ENABLE_INOTIFY)

IF(ENABLE_DBUS)
  add_definitions(-DHAVE_DBUS)
  MESSAGE("-- dbus support enabled")
  set(strigidaemon_LIBS ${strigidaemon_LIBS} dbusserver)
ENDIF(ENABLE_DBUS)

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
    MESSAGE("-- log4cxx support enabled")
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

IF (ENABLE_LOG4CXX AND NOT Log4cxx_FOUND)
    MESSAGE("-- log4cxx WONT be enabled")
ENDIF (ENABLE_LOG4CXX AND NOT Log4cxx_FOUND)

if (CLucene_FOUND)
    add_definitions(-DHAVE_CLUCENE)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} cluceneindex)
    include_directories( ../luceneindexer )
endif (CLucene_FOUND)
if (HyperEstraier_FOUND)
    add_definitions(-DHAVE_ESTRAIER)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} estraierindex)
    include_directories( ../estraierindexer )
endif (HyperEstraier_FOUND)

include_directories( eventlistener .)

# daemonconfiguration has been removed until the next release
add_executable(strigidaemon daemon.cpp socketserver.cpp interface.cpp
	indexscheduler.cpp clientinterface.cpp
	filtermanager.cpp strigilogging.h strigithread.cpp)

set(strigidaemon_LIBS streamindexer eventlistener
	${CMAKE_THREAD_LIBS_INIT} ${strigidaemon_LIBS})
target_link_libraries(strigidaemon ${strigidaemon_LIBS})

add_library(searchclient SHARED socketclient.cpp clientinterface.cpp
        asyncsocket.cpp asyncsocketclient.cpp)
set_target_properties(searchclient PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})

install(TARGETS strigidaemon searchclient
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
install(FILES asyncsocketclient.h asyncsocket.h clientinterface.h
	DESTINATION include)

