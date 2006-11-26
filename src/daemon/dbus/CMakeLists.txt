include_directories(..)

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(test_LIBS ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

add_custom_command(OUTPUT dbusclientinterface.cpp
	COMMAND perl ARGS makecode.pl vandenoever.strigi ../clientinterface.h
	MAIN_DEPENDENCY ../clientinterface.h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/daemon/dbus)

add_custom_command(OUTPUT dbustestinterface.cpp
	COMMAND perl ARGS makecode.pl vandenoever.dbustest testinterface.h
	MAIN_DEPENDENCY testinterface.h
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/daemon/dbus)

add_library(dbusserver dbusserver.cpp dbusmessagereader.cpp
	dbusmessagewriter.cpp dbusobjectcallhandler.cpp dbusclientinterface.cpp
	dbustestinterface.cpp testinterface.cpp)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbusserver ${DBUS_LDFLAGS})

add_executable(testserver testserver.cpp ../strigithread.cpp)
target_link_libraries(testserver dbusserver ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS} streams)

add_executable(dbustest dbustest.cpp)
target_link_libraries(dbustest ${DBUS_LDFLAGS} ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS})
