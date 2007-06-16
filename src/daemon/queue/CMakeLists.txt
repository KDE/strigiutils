
include_directories( ../streamanalyzer ../combinedindexer ../streams ../xsd dbus
	${strigi_BINARY_DIR}/src/streams/compat ../streams/compat)

add_executable(newdaemon main.cpp jobqueue.cpp)

set(newdaemon_LIBS
	combinedindex
	eventlistener
	streamanalyzer
	${CMAKE_THREAD_LIBS_INIT}
	${strigidaemon_LIBS}
)
target_link_libraries(newdaemon pthread)#${newdaemon_LIBS})

