include_directories (   .
                        ${strigi_SOURCE_DIR}/src/streamanalyzer
                        ${strigi_SOURCE_DIR}/src/combinedindexer
                    )

add_library(unittestfunctions unittestfunctions.cpp)

target_link_libraries (unittestfunctions combinedindex)