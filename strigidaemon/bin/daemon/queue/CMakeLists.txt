add_executable(newdaemon main.cpp jobqueue.cpp)

set(newdaemon_LIBS
	combinedindex
	eventlistener
	streamanalyzer
	${CMAKE_THREAD_LIBS_INIT}
	${strigidaemon_LIBS}
)
target_link_libraries(newdaemon ${CMAKE_THREAD_LIBS_INIT})

