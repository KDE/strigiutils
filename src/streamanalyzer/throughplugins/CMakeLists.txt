include_directories( ..  ../../streams ${strigi_BINARY_DIR}/src/streams/compat)

if (XATTR_FOUND)
  include_directories(${XATTR_INCLUDE_DIR})
  add_library(xattr MODULE xattrthroughanalyzer.cpp)
  set_target_properties(xattr PROPERTIES
    PREFIX strigita_
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_VERSION_MAJOR})
  target_link_libraries(xattr streamanalyzer)
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

add_library(pcx MODULE pcxthroughanalyzer.cpp)
set_target_properties(pcx PROPERTIES
  PREFIX strigita_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(pcx streamanalyzer)
install(TARGETS pcx LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

add_library(xbm MODULE xbmthroughanalyzer.cpp)
set_target_properties(xbm PROPERTIES
  PREFIX strigita_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(xbm streamanalyzer)
install(TARGETS xbm LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

# disabled until a good check for making sure the file is a tga
#add_library(tga MODULE tgathroughanalyzer.cpp)
#set_target_properties(tga PROPERTIES
#  PREFIX strigita_
#  VERSION ${STRIGI_VERSION}
#  SOVERSION ${STRIGI_VERSION_MAJOR})
#target_link_libraries(tga streamanalyzer)
#install(TARGETS tga LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

add_library(au MODULE authroughanalyzer.cpp)
set_target_properties(au PROPERTIES
  PREFIX strigita_
  VERSION ${STRIGI_VERSION}
  SOVERSION ${STRIGI_VERSION_MAJOR})
target_link_libraries(au streamanalyzer)
install(TARGETS au LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
