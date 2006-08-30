include_directories(..)
add_library(dbusserver dbusserver.cpp dbusmessagereader.cpp
	dbusmessagewriter.cpp dbusobjectcallhandler.cpp dbusclientinterface.cpp)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbusserver dbus-1)
