MACRO(ADD_STRIGIEA libname files)
  if(WIN32)
    # this is needed to have mingw, cygwin and msvc libs installed in one directory
    if(MSVC)
      set(prefix msvc_strigiea_)
    elseif(CYGWIN)  
      set(prefix cyg_strigiea_)
    elseif(MINGW)
      # mingw
      set(prefix mingw_strigiea_)
    endif(MSVC)

  else(WIN32)
    set(prefix strigiea_)
  endif(WIN32)
  add_library(${libname} MODULE ${files})
  set_target_properties(${libname} PROPERTIES
    PREFIX ${prefix})
  target_link_libraries(${libname} streamanalyzer)
  install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGIEA)

if(EXIV2_FOUND)
  ADD_STRIGIEA(jpeg jpegendanalyzer.cpp)
  target_link_libraries(jpeg ${EXIV2_LIBRARIES})
endif(EXIV2_FOUND)

if(XINE_FOUND)
  ADD_STRIGIEA(xine xineendanalyzer.cpp)
  target_link_libraries(xine ${XINE_LIBRARY})
endif(XINE_FOUND)

if(FFMPEG_FOUND)
  add_definitions(-D__STDC_CONSTANT_MACROS)
  ADD_STRIGIEA(ffmpeg ffmpegendanalyzer.cpp)
  #set_target_properties( ffmpeg PROPERTIES COMPILE_FLAGS "${FFMPEG_DEFINITIONS}" )
  target_link_libraries(ffmpeg ${FFMPEG_LIBRARIES})
endif(FFMPEG_FOUND)
