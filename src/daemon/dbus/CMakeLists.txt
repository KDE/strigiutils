include_directories(.. ${DBus_INCLUDE_DIR})
add_library(dbusserver dbusserver.cpp dbusmessagereader.cpp
	dbusmessagewriter.cpp)
target_link_libraries(dbusserver dbus-1)
