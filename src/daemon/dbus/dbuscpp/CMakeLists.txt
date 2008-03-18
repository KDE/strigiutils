#include_directories(test)

IF(WIN32)
include_directories(${DBUS_INCLUDE_DIR})
ENDIF(WIN32)

add_library(dbuscpp dbushandler.cpp dbusmessagereader.cpp
    dbusmessagewriter.cpp dbusobjectcallhandler.cpp
)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DBUS_CFLAGS}")

IF(WIN32)
target_link_libraries(dbuscpp ${DBUS_LIBRARY})
ELSE(WIN32)
target_link_libraries(dbuscpp ${DBUS_LDFLAGS})
ENDIF(WIN32)
