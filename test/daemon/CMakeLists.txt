STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories (.  ${INC_DIR}
                        ${strigi_SOURCE_DIR}/src/xsd
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/streams/compat 
                        ${strigi_BINARY_DIR}/src/streams/compat
)

add_library(daemonconfiguratortest SHARED daemonconfiguratortest.cpp)
target_link_libraries (daemonconfiguratortest daemonconfigurator)
