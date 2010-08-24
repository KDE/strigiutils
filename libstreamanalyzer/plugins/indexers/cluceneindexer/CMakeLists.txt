include (CheckCXXCompilerFlag)

IF(WIN32)
    # why only here and not for the complete project?
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(WIN32)

# We need to compile the clucene index plugin with default visibility, because
# CLucene does not export the symbol for CLuceneError. If this symbol is not
# exported, the exception cannot be caught and can cause programs that use
# libstreamanalyzer to crash.
# Once CLucene has '__attribute__ ((visibility("default")))' before
# 'class CLuceneError', we can revert to -fvisibility=hidden.
IF(__STRIGI_HAVE_GCC_VISIBILITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
ENDIF(__STRIGI_HAVE_GCC_VISIBILITY)

set(cluceneindex_SRCS
    cluceneindexmanager.cpp
    cluceneindexreader.cpp
    cluceneindexwriter.cpp
    jsgzipcompressstream.cpp
    tcharutils.cpp
)
include_directories(${CLUCENE_INCLUDE_DIR} ${CLUCENE_LIBRARY_DIR})

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
target_link_libraries(clucene ${CLUCENE_LIBRARY} streamanalyzer)
install(TARGETS clucene LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

if(BUILD_UTILS)
  add_executable(luceneindexer luceneindexer.cpp)
  target_link_libraries(luceneindexer streamanalyzer)
  install(TARGETS luceneindexer RUNTIME DESTINATION bin)
endif(BUILD_UTILS)

add_subdirectory(indexdump)
add_subdirectory(simplereader)

# TODO: fix the tests in the directory 'tests' and reenable them
if (CppUnit_FOUND)
  add_subdirectory(tests)
endif (CppUnit_FOUND)
