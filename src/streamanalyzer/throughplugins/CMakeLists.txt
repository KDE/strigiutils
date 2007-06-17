include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams/compat)

MACRO(ADD_STRIGITA libname files)
  if(WIN32)
    # this is needed to have mingw and msvc libs installed in one directory
    if(MSVC)
      set(prefix msvc_strigita_)
    else(MSVC)
      # mingw
      set(prefix mingw_strigita_)
    endif(MSVC)
  else(WIN32)
    set(prefix strigita_)
  endif(WIN32)
  add_library(${libname} MODULE ${files})
  set_target_properties(${libname} PROPERTIES
    PREFIX ${prefix}
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  target_link_libraries(${libname} streamanalyzer)
  install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGITA)


if (XATTR_FOUND)
  include_directories(${XATTR_INCLUDE_DIR})
  ADD_STRIGITA(xattr xattrthroughanalyzer.cpp)
endif (XATTR_FOUND)

ADD_STRIGITA(pcx pcxthroughanalyzer.cpp)
ADD_STRIGITA(xbm xbmthroughanalyzer.cpp)
ADD_STRIGITA(au  authroughanalyzer.cpp)
ADD_STRIGITA(gif gifthroughanalyzer.cpp)

# disabled until a good check for making sure the file is a tga
#ADD_STRIGITA(tga  tgathroughanalyzer.cpp)
