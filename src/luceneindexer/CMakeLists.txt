include (CheckCXXCompilerFlag)

# TODO: fix the tests in the directory 'tests' and reenable them
subdirs(indexdump simplereader tests)

include_directories(
    ${strigi_SOURCE_DIR}/src/streamanalyzer
    ${strigi_SOURCE_DIR}/src/streams
    ${strigi_BINARY_DIR}/src/streams
    ${strigi_SOURCE_DIR}/src/streams/strigi
    ${CLUCENE_LIBRARY_DIR}
    ${CLUCENE_INCLUDE_DIR}
    ${ICONV_INCLUDE_DIR}
)

IF(WIN32)
    # why only here and not for the complete project?
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(WIN32)

# CLucene requires exception support
IF(NOT WIN32)
    IF (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(NOT WIN32)
# In the past, we though we needed to use -fvisibility=default for compiling
# this library. This appears not to be the case anymore.
#IF(__STRIGI_HAVE_GCC_VISIBILITY)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
#ENDIF(__STRIGI_HAVE_GCC_VISIBILITY)

set(cluceneindex_SRCS
    cluceneindexmanager.cpp
    cluceneindexreader.cpp
    cluceneindexwriter.cpp
    jsgzipcompressstream.cpp
    tcharutils.cpp
)

if(WIN32)
  # this is needed to have mingw, cygwin and msvc libs installed in one directory
  if(MSVC)
    set(prefix msvc_strigiindex_)
  elseif(CYGWIN)
    set(prefix cyg_strigiindex_)
  elseif(MINGW)
    set(prefix mingw_strigiindex_)
  endif(MSVC)
else(WIN32)
  set(prefix strigiindex_)
endif(WIN32)
add_library(clucene MODULE ${cluceneindex_SRCS})
set_target_properties(clucene PROPERTIES PREFIX ${prefix})
target_link_libraries(clucene streamanalyzer ${CLUCENE_LIBRARY})
install(TARGETS clucene LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

add_executable(luceneindexer luceneindexer.cpp)
target_link_libraries(luceneindexer streamanalyzer)

install(TARGETS luceneindexer RUNTIME DESTINATION bin)
