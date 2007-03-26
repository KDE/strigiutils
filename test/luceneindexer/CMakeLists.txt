STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories (. .. ${INC_DIR}
                     ../indextests
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams/compat
                     ${strigi_SOURCE_DIR}/src/streams/compat
                     ${CLUCENE_INCLUDE_DIR}
)

add_library(lucenetest SHARED   luceneindexmanagertest.cpp
                                ../indextests/indexmanagertester.cpp
                                luceneindexreadertest.cpp
                                ../indextests/indexreadertester.cpp
                                luceneindexwritertest.cpp
                                ../indextests/indexwritertester.cpp )

target_link_libraries (lucenetest cluceneindex)