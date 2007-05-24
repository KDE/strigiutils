include_directories (.
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams/compat
                     ${strigi_SOURCE_DIR}/src/streams/compat
                     ../utils
)

add_library(streamanalyzertesters diranalyzertester.cpp)

target_link_libraries (streamanalyzertesters unittestfunctions )