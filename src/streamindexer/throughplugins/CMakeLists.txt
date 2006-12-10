include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams/compat)

if (XATTR_FOUND)
  include_directories(${XATTR_INCLUDE_DIR})
  add_library(xattr MODULE xattrthroughanalyzer.cpp)
  set_target_properties(xattr PROPERTIES
    PREFIX strigita_
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  install(TARGETS xattr LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
endif (XATTR_FOUND)

if (Magic_FOUND)
  include_directories(${MAGIC_INCLUDE_DIR})
  add_library(magic MODULE mimetypethroughanalyzer.cpp)
  set_target_properties(magic PROPERTIES
    PREFIX strigita_
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  target_link_libraries(magic ${MAGIC_LIBRARIES})
  install(TARGETS magic LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
endif (Magic_FOUND)

