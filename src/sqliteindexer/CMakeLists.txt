subdirs(tests)

include_directories( ../streamanalyzer ../streams ../streams/strigi
	${SQLITE_LIBRARY_DIR} ${SQLITE_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams)

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
add_library(sqlite MODULE
	sqliteindexmanager.cpp
        sqliteindexreader.cpp
	sqliteindexwriter.cpp
)
set_target_properties(sqlite PROPERTIES PREFIX ${prefix})
target_link_libraries(sqlite ${SQLITE_LIBRARIES})
install(TARGETS sqlite LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
