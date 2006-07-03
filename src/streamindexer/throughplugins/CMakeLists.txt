include_directories( .. )

if (HAVE_SYS_XATTR_H)
  add_library(xattr MODULE xattrthroughanalyzer.cpp)
  set_target_properties(xattr PROPERTIES PREFIX strigita_)
#  target_link_libraries(xattr streams)
  install(TARGETS xattr LIBRARY DESTINATION lib)
endif (HAVE_SYS_XATTR_H)

if (FOUND_MAGIC)
  include_directories(MAGIC_INCLUDE_DIR)
  add_library(magic MODULE mimetypethroughanalyzer.cpp)
  set_target_properties(magic PROPERTIES PREFIX strigita_)
  target_link_libraries(magic ${MAGIC_LIBRARIES})
  install(TARGETS magic LIBRARY DESTINATION lib)
endif (FOUND_MAGIC)

