STRING(REGEX REPLACE /test /src INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories (. .. ${INC_DIR}
                     ../indextests
                     ${strigi_SOURCE_DIR}/src/streamanalyzer
                     ${strigi_SOURCE_DIR}/src/streams
                     ${strigi_BINARY_DIR}/src/streams/compat
                     ${strigi_SOURCE_DIR}/src/streams/compat
                     ${EST_INCLUDE_DIR}
)

add_library(estraiertest SHARED   estraierindexmanagertest.cpp
                                ../indextests/indexmanagertester.cpp
                                estraierindexreadertest.cpp
                                ../indextests/indexreadertester.cpp
                                estraierindexwritertest.cpp
                                ../indextests/indexwritertester.cpp )

target_link_libraries (estraiertest estraierindex)
add_test (estraiertest estraiertest)