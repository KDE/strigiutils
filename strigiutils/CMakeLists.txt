include_directories (   .
                        ${strigi_BINARY_DIR}/src/streams
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/combinedindexer
                    )

add_library(unittestfunctions STATIC unittestfunctions.cpp)

target_link_libraries (unittestfunctions combinedindex)
