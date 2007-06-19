include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams/compat)


MACRO(ADD_STRIGIEA libname files)
  if(WIN32)
    # this is needed to have mingw and msvc libs installed in one directory
    if(MSVC)
      set(prefix msvc_strigiea_)
    else(MSVC)
      # mingw
      set(prefix mingw_strigiea_)
    endif(MSVC)
  else(WIN32)
    set(prefix strigiea_)
  endif(WIN32)
  add_library(${libname} MODULE ${files})
  set_target_properties(${libname} PROPERTIES
    PREFIX ${prefix}
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  target_link_libraries(${libname} streamanalyzer)
  install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGIEA)

if(EXIV2_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
  include_directories(${EXIV2_INCLUDE_DIR})
  ADD_STRIGIEA(jpeg jpegendanalyzer.cpp)
  target_link_libraries(jpeg ${EXIV2_LIBRARIES})
endif(EXIV2_FOUND)

