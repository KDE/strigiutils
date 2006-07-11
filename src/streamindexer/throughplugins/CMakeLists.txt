include_directories( .. )

if (HAVE_ATTR_XATTR_H)
  add_library(xattr MODULE xattrthroughanalyzer.cpp)
  set_target_properties(xattr PROPERTIES
    PREFIX strigita_
    SOVERSION ${STRIGI_VERSION})
  install(TARGETS xattr LIBRARY DESTINATION lib)
endif (HAVE_ATTR_XATTR_H)

if (Magic_FOUND)
  include_directories(MAGIC_INCLUDE_DIR)
  add_library(magic MODULE mimetypethroughanalyzer.cpp)
  set_target_properties(magic PROPERTIES
    PREFIX strigita_
    SOVERSION ${STRIGI_VERSION})
  target_link_libraries(magic ${MAGIC_LIBRARIES})
  install(TARGETS magic LIBRARY DESTINATION lib)
endif (Magic_FOUND)

