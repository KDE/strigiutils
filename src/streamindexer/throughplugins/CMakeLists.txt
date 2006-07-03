include_directories( ../../streams ..)

if (HAVE_SYS_XATTR_H)
  add_library(strigita_xattr MODULE xattrthroughanalyzer.cpp)
  target_link_libraries(strigita_xattr streams)
endif (HAVE_SYS_XATTR_H)
