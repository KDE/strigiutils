set(DIRS eventlistener events xesam queue)

IF(ENABLE_DBUS)
  set(DIRS ${DIRS} dbus)
ENDIF(ENABLE_DBUS)

subdirs(${DIRS})

include_directories( ../streamanalyzer ../combinedindexer ../streams ../xsd dbus
	${strigi_BINARY_DIR}/src/streams ../streams/strigi xesam)

IF(ENABLE_INOTIFY)
  add_definitions(-DHAVE_INOTIFY)
  MESSAGE("-- inotify support enabled")
ENDIF(ENABLE_INOTIFY)

IF(ENABLE_POLLING)
  add_definitions(-DHAVE_POLLING)
  MESSAGE("-- polling support enabled")
ENDIF(ENABLE_POLLING)

IF(ENABLE_DBUS)
  add_definitions(-DHAVE_DBUS)
  MESSAGE("-- dbus support enabled")
  set(strigidaemon_LIBS ${strigidaemon_LIBS} dbusserver)
ENDIF(ENABLE_DBUS)

IF (ICONV_FOUND)
  add_definitions(-DLIBXML_ICONV_ENABLED)
ENDIF (ICONV_FOUND)

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} ${LOG4CXX_LIBRARIES})
    set(strigiconfupdater_LIBS ${strigiconfupdater_LIBS} ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
    MESSAGE("-- log4cxx support enabled")
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

IF (ENABLE_LOG4CXX AND NOT Log4cxx_FOUND)
    MESSAGE("-- log4cxx WONT be enabled")
ENDIF (ENABLE_LOG4CXX AND NOT Log4cxx_FOUND)

include_directories(eventlistener .)

add_executable(strigidaemon
	daemon.cpp
	indexscheduler.cpp
	interface.cpp
	socketserver.cpp
	strigilogging.h
	strigithread.cpp
	queue/jobqueue.cpp
)

IF (CMAKE_LIB_SOCKET_HAS_CONNECT)
    target_link_libraries(strigidaemon socket nsl)
ENDIF (CMAKE_LIB_SOCKET_HAS_CONNECT)

#add_executable(strigiconfupdater configupdater.cpp strigilogging.h)

set(strigidaemon_LIBS
	combinedindex
	eventlistener
	streamanalyzer
	xesam
	${CMAKE_THREAD_LIBS_INIT}
	${strigidaemon_LIBS}
)
target_link_libraries(strigidaemon ${strigidaemon_LIBS} daemonconfigurator)

#target_link_libraries(strigiconfupdater ${strigiconfupdater_LIBS} filters
#                      daemonconfigurator streamanalyzer)

add_library(searchclient SHARED socketclient.cpp asyncsocket.cpp
	asyncsocketclient.cpp)
set_target_properties(searchclient PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR}
    DEFINE_SYMBOL MAKE_SEARCHCLIENT_LIB
)

IF(CMAKE_LIB_SOCKET_HAS_CONNECT)
    target_link_libraries(searchclient socket nsl)
ENDIF(CMAKE_LIB_SOCKET_HAS_CONNECT)

install(TARGETS strigidaemon searchclient #strigiconfupdater
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_DESTINATION})
install(FILES asyncsocketclient.h asyncsocket.h clientinterface.h
	DESTINATION include/strigi)

# create a library from the xml io files
add_library(daemonconfigurator daemonconfigurator.cpp)
target_link_libraries( daemonconfigurator strigidaemonconfiguration)
