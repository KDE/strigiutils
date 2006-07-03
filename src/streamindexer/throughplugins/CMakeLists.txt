include_directories( .. )

if (HAVE_SYS_XATTR_H)
  add_library(strigita_xattr MODULE xattrthroughanalyzer.cpp)
  target_link_libraries(strigita_xattr streams)
  install(TARGETS strigita_xattr
	LIBRARY DESTINATION lib)
endif (HAVE_SYS_XATTR_H)
