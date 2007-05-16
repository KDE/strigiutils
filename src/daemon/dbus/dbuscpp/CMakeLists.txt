include_directories(test)

add_library(dbuscpp dbushandler.cpp dbusmessagereader.cpp
    dbusmessagewriter.cpp dbusobjectcallhandler.cpp
)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbuscpp ${DBUS_LDFLAGS})

