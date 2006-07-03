include_directories( .. )

if (HAVE_SYS_XATTR_H)
  add_library(xattr MODULE xattrthroughanalyzer.cpp)
  set_target_properties(xattr PROPERTIES PREFIX strigita_)
  target_link_libraries(xattr streams)
  install(TARGETS xattr
	LIBRARY DESTINATION lib)
endif (HAVE_SYS_XATTR_H)
