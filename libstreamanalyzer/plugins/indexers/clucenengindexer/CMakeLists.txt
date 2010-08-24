include (CheckCXXCompilerFlag)

IF(WIN32)
    # why only here and not for the complete project?
    ADD_DEFINITIONS(-D_UNICODE)
    ADD_DEFINITIONS(-DUNICODE)
ENDIF(WIN32)

set(cluceneindex_SRCS
    cluceneindexmanager.cpp
    cluceneindexreader.cpp
    cluceneindexwriter.cpp
    tcharutils.cpp
)
include_directories(${CLUCENE1_INCLUDE_DIRS})

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

add_library(cluceneng MODULE ${cluceneindex_SRCS})
set_target_properties(cluceneng PROPERTIES PREFIX ${prefix})
target_link_libraries(cluceneng ${CLUCENE1_LDFLAGS} streamanalyzer)
install(TARGETS cluceneng LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

if(BUILD_UTILS)
  add_executable(lucene2indexer luceneindexer.cpp)
  target_link_libraries(lucene2indexer streamanalyzer)
  install(TARGETS lucene2indexer RUNTIME DESTINATION bin)
endif(BUILD_UTILS)

add_subdirectory(indexdump)
#add_subdirectory(tests)

# TODO: fix the tests in the directory 'tests' and reenable them
if (CppUnit_FOUND)
  add_subdirectory(tests)
endif (CppUnit_FOUND)

