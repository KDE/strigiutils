add_subdirectory(eventlistener)
add_subdirectory(queue)
add_subdirectory(xesam)
add_subdirectory(xsd)

set(strigidaemon_SRCS 
	daemon.cpp
	indexscheduler.cpp
	interface.cpp
	queue/jobqueue.cpp
	socketserver.cpp
	strigithread.cpp
	daemonconfigurator.cpp
)
set(strigidaemon_LIBS
	combinedindex
	eventlistener
	xesam
	strigidaemonconfiguration
	streamanalyzer
	${LOG4CXX_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)
if(ENABLE_DBUS)
  add_definitions(-DHAVE_DBUS)
  add_subdirectory(dbus)
  set(strigidaemon_LIBS ${strigidaemon_LIBS} dbusserver)
endif(ENABLE_DBUS)

add_library(libstrigidaemon STATIC ${strigidaemon_SRCS})
target_link_libraries(libstrigidaemon ${strigidaemon_LIBS})
