include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams)

MACRO(ADD_STRIGILA libname files)
  if(WIN32)
    # this is needed to have mingw, cygwin and msvc libs installed in one directory
    if(MSVC)
      set(prefix msvc_strigila_)
    elseif(CYGWIN)
      set(prefix cyg_strigila_)
    elseif(MINGW)
      set(prefix mingw_strigila_)
    endif(MSVC)
  else(WIN32)
    set(prefix strigila_)
  endif(WIN32)
  add_library(${libname} MODULE ${files})
  set_target_properties(${libname} PROPERTIES
    PREFIX ${prefix}
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  target_link_libraries(${libname} streamanalyzer)
  install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGILA)

ADD_STRIGILA(xpm xpmlineanalyzer.cpp)
ADD_STRIGILA(deb deblineanalyzer.cpp)
ADD_STRIGILA(cpp cpplineanalyzer.cpp)
