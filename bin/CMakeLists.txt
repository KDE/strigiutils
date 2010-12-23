# regex.c is no longer provided
if(HAVE_REGEX_H)
  add_subdirectory(deepfind)
endif(HAVE_REGEX_H)
if(NOT MSVC)
  add_subdirectory(xmlindexer)
endif(NOT MSVC)
