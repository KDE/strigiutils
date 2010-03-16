add_library(dbuscpp STATIC dbushandler.cpp dbusmessagereader.cpp
    dbusmessagewriter.cpp dbusobjectcallhandler.cpp
)

IF(WIN32)
	target_link_libraries(dbuscpp ${DBUS_LIBRARY})
ELSE(WIN32)
	target_link_libraries(dbuscpp ${DBUS_LDFLAGS})
ENDIF(WIN32)
