include_directories(.. ${CMAKE_CURRENT_BINARY_DIR})

IF (ENABLE_LOG4CXX AND Log4cxx_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
    set(test_LIBS ${LOG4CXX_LIBRARIES})
    include_directories( ${LOG4CXX_INCLUDE_DIR} )
ENDIF (ENABLE_LOG4CXX AND Log4cxx_FOUND)

# if available, use Perl for generating the dbus interfaces.
# otherwise, just copy them into the build directory
find_package(Perl)

IF (PERL_FOUND)
    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
        COMMAND ${PERL_EXECUTABLE}
        ARGS    ${CMAKE_CURRENT_SOURCE_DIR}/makecode.pl
                vandenoever.strigi.ClientInterface
                ${strigi_SOURCE_DIR}/src/daemon/clientinterface.h
        MAIN_DEPENDENCY ${strigi_SOURCE_DIR}/src/daemon/clientinterface.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
        COMMAND ${PERL_EXECUTABLE}
        ARGS    ${CMAKE_CURRENT_SOURCE_DIR}/makecode.pl
                vandenoever.dbustest.DBusTest
                ${CMAKE_CURRENT_SOURCE_DIR}/testinterface.h
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/testinterface.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
ELSE (PERL_FOUND)
    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
        COMMAND ${CMAKE_COMMAND}
        ARGS    -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbusclientinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.h"
        COMMAND ${CMAKE_COMMAND}
        ARGS    -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbusclientinterface.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
        COMMAND ${CMAKE_COMMAND}
        ARGS    -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbustestinterface.h"
                "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.h"
        COMMAND ${CMAKE_COMMAND}
        ARGS    -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbustestinterface.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
    )
ENDIF (PERL_FOUND)

set_source_files_properties(
    "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.h"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.h"
    "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
    GENERATED
)

add_library(dbusserver dbusserver.cpp dbusmessagereader.cpp
    dbusmessagewriter.cpp dbusobjectcallhandler.cpp testinterface.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
)
set(CMAKE_CXX_FLAGS ${DBUS_CFLAGS})
target_link_libraries(dbusserver ${DBUS_LDFLAGS})

add_executable(testserver testserver.cpp ../strigithread.cpp)
target_link_libraries(testserver dbusserver ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS} streams)

add_executable(dbustest dbustest.cpp)
target_link_libraries(dbustest ${DBUS_LDFLAGS} ${CMAKE_THREAD_LIBS_INIT} ${test_LIBS})


# Convenience target to copy the generated D-Bus interfaces
# into the generated/ folder, so they can be imported into SVN.
# From the current directory, call 'make copy-generated' to copy them.
add_custom_target( copy-generated
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbusclientinterface.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/dbusclientinterface.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbusclientinterface.cpp"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbustestinterface.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/dbustestinterface.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/generated/dbustestinterface.cpp"
)
