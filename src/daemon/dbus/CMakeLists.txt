include_directories(..)

add_custom_command(OUTPUT dbusclientinterface.cpp
	COMMAND perl ARGS makecode.pl vandenoever.strigi ../clientinterface.h
	MAIN_DEPENDENCY ../clientinterface.h
	WORKING_DIRECTORY ../src/daemon/dbus)

add_custom_command(OUTPUT dbustestinterface.cpp
	COMMAND perl ARGS makecode.pl vandenoever.dbustest testinterface.h
	MAIN_DEPENDENCY testinterface.h
	WORKING_DIRECTORY ../src/daemon/dbus)

add_library(dbusserver dbusserver.cpp dbusmessagereader.cpp
	dbusmessagewriter.cpp dbusobjectcallhandler.cpp dbusclientinterface.cpp
	dbustestinterface.cpp testinterface.cpp)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbusserver dbus-1)
