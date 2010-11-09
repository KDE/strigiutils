subdirs(dbuscpp)

# if available, use Perl for generating the dbus interfaces.
# otherwise, just copy them into the build directory
find_package(Perl)
find_package(LibDBus)

MACRO(CreateDBusBinding HEADERFILE INAME INTERFACE EXTRAINCLUDE)
    IF (PERL_FOUND)
        add_custom_command(
            OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
            COMMAND ${PERL_EXECUTABLE}
            ARGS    "${CMAKE_CURRENT_SOURCE_DIR}/dbuscpp/makecode.pl"
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
                    "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbus${INAME}.cpp"
                    "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
        )
    ENDIF (PERL_FOUND)
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.h"
        "${CMAKE_CURRENT_BINARY_DIR}/dbus${INAME}.cpp"
        PROPERTIES GENERATED true)
ENDMACRO(CreateDBusBinding)

CreateDBusBinding("${CMAKE_CURRENT_SOURCE_DIR}/../../../include/strigi/clientinterface.h"
        "clientinterface" "vandenoever.strigi"
        "dbus/dbusserialization.h")
CreateDBusBinding("${CMAKE_CURRENT_SOURCE_DIR}/testinterface.h"
        "testinterface" "vandenoever.strigi" "")
CreateDBusBinding(
        "${CMAKE_CURRENT_SOURCE_DIR}/../xesam/xesamlivesearchinterface.h"
        "xesamlivesearchinterface" "org.freedesktop.xesam.Search"
        "dbus/dbusserialization.h")

add_library(dbusserver STATIC testinterface.cpp dbusserver.cpp
    dbusserialization.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusxesamlivesearchinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DBUS_CFLAGS}")
target_link_libraries(dbusserver dbuscpp)

add_executable(testserver testserver.cpp ../strigithread.cpp)
target_link_libraries(testserver dbusserver ${CMAKE_THREAD_LIBS_INIT} ${DBUS_LIBRARIES} ${LOG4CXX_LIBRARIES} streamanalyzer)

add_executable(dbustest dbustest.cpp)
target_link_libraries(dbustest  ${CMAKE_THREAD_LIBS_INIT} ${LOG4CXX_LIBRARIES} ${DBUS_LIBRARIES})

configure_file(vandenoever.strigi.service.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/vandenoever.strigi.service)
configure_file(org.freedesktop.xesam.searcher.service.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.xesam.searcher.service)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vandenoever.strigi.service ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.xesam.searcher.service DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/services )
