subdirs(xsd eventlistener)

include_directories( ../streamindexer ../streams xsd )

IF(ENABLE_INOTIFY)
  add_definitions(-DHAVE_INOTIFY)
  MESSAGE("-- inotify support enabled")
ENDIF(ENABLE_INOTIFY)

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

include_directories( eventlistener )
if(LOG4CXX_FOUND)
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
endif(LOG4CXX_FOUND)

add_executable(strigidaemon daemon.cpp server.cpp socketserver.cpp interface.cpp
	indexscheduler.cpp clientinterface.cpp daemonconfiguration.cpp
	strigilogging.h)

set(strigidaemon_LIBS streamindexer eventlistener daemonconfiguration
	${CMAKE_THREAD_LIBS_INIT} ${strigidaemon_LIBS})
if(LOG4CXX_FOUND)
    set(strigidaemon_LIBS ${strigidaemon_LIBS} ${LOG4CXX_LIBRARIES})
endif(LOG4CXX_FOUND)
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

