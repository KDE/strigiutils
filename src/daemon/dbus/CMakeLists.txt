subdirs(dbuscpp)
include_directories(.. dbuscpp ${CMAKE_CURRENT_BINARY_DIR})

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(test_LIBS ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

# if available, use Perl for generating the dbus interfaces.
# otherwise, just copy them into the build directory
find_package(Perl)

MACRO(CreateDBusBinding HEADERFILE INAME INTERFACE EXTRAINCLUDE)
    IF (PERL_FOUND)
        add_custom_command(
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
            COMMAND ${PERL_EXECUTABLE}
            ARGS    "${strigi_SOURCE_DIR}/src/daemon/dbus/dbuscpp/makecode.pl"
                    ${INTERFACE} 
                    ${HEADERFILE}
                    ${EXTRAINCLUDE}
            MAIN_DEPENDENCY ${HEADERFILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbus${INAME}.h"
        )
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbus${INAME}.cpp"
        )
    ELSE (PERL_FOUND)
        add_custom_command(
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
            COMMAND ${CMAKE_COMMAND}
            ARGS    -E copy_if_different
                    "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbus${INAME}.h"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
            COMMAND ${CMAKE_COMMAND}
            ARGS    -E copy_if_different
                    "${CMAKE_CURRENT_SOURCE_DIR}/generated/${INAME}.cpp"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
        )
    ENDIF (PERL_FOUND)
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
        "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp")
ENDMACRO(CreateDBusBinding)

CreateDBusBinding("${strigi_SOURCE_DIR}/src/daemon/clientinterface.h"
        "clientinterface" "vandenoever.strigi"
        "${strigi_SOURCE_DIR}/src/daemon/dbus/dbusserialization.h")
CreateDBusBinding("${strigi_SOURCE_DIR}/src/daemon/dbus/testinterface.h"
        "testinterface" "vandenoever.strigi" "")
CreateDBusBinding(
        "${strigi_SOURCE_DIR}/src/daemon/xesam/xesamlivesearchinterface.h"
        "xesamlivesearchinterface" "org.freedesktop.xesam.search"
        "${strigi_SOURCE_DIR}/src/daemon/dbus/dbusserialization.h")

add_library(dbusserver testinterface.cpp dbusserver.cpp
    dbusserialization.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusxesamlivesearchinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbusserver dbuscpp)

add_executable(testserver testserver.cpp ../strigithread.cpp)
target_link_libraries(testserver dbusserver ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS} streams)

#add_executable(dbustest dbustest.cpp)
#target_link_libraries(dbustest ${DBUS_LDFLAGS} ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS})


