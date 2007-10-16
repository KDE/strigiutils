subdirs(tests)

include_directories( ../streamanalyzer ../streams ${EST_INCLUDE_DIR}
	${strigi_BINARY_DIR}/src/streams
	${strigi_SOURCE_DIR}/src/streams/strigi)
link_directories(${EST_LIBDIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EST_CFLAGS}")

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
add_library(estraier MODULE
	estraierindexmanager.cpp
        estraierindexreader.cpp
	estraierindexwriter.cpp
)
set_target_properties(estraier PROPERTIES PREFIX ${prefix})
target_link_libraries(estraier streamanalyzer ${EST_LIBS})
install(TARGETS estraier LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
