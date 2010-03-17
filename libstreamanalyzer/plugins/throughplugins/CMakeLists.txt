MACRO(ADD_STRIGITA libname files)
  if(WIN32)
    # this is needed to have mingw, cygwin and msvc libs installed in one directory
    if(MSVC)
      set(prefix msvc_strigita_)
    elseif(CYGWIN)
      set(prefix cyg_strigita_)
    elseif(MINGW)
      set(prefix mingw_strigita_)
    endif(MSVC)
  else(WIN32)
    set(prefix strigita_)
  endif(WIN32)
  add_library(${libname} MODULE ${files})
  set_target_properties(${libname} PROPERTIES
    PREFIX ${prefix})
  target_link_libraries(${libname} streamanalyzer)
  install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGITA)

if (XATTR_FOUND)
#  ADD_STRIGITA(xattr xattrthroughanalyzer.cpp)
endif (XATTR_FOUND)

ADD_STRIGITA(pcx pcxthroughanalyzer.cpp)
ADD_STRIGITA(xbm xbmthroughanalyzer.cpp)
ADD_STRIGITA(au  authroughanalyzer.cpp)
ADD_STRIGITA(gif gifthroughanalyzer.cpp)
ADD_STRIGITA(ico icothroughanalyzer.cpp)
ADD_STRIGITA(sid sidthroughanalyzer.cpp)
ADD_STRIGITA(rgb rgbthroughanalyzer.cpp)
ADD_STRIGITA(dds ddsthroughanalyzer.cpp)

# Bug 164296: these two analyzers are dummy leftovers.They make sure that
# possible original, broken analyzers are overwritten/neutralized.
ADD_STRIGITA(avi dummy.cpp)
ADD_STRIGITA(wav dummy.cpp)

# disabled until a good check for making sure the file is a tga
#ADD_STRIGITA(tga  tgathroughanalyzer.cpp)
