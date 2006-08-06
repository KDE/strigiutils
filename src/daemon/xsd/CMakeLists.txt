IF(XSD_FOUND)
  ADD_CUSTOM_COMMAND(OUTPUT strigidaemon.cxx
    COMMAND ${XSD_EXECUTABLE} cxx-tree --generate-serialization strigidaemon.xsd
    DEPENDS strigidaemon.xsd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/daemon/xsd )
ENDIF(XSD_FOUND)

include_directories(${XERCESC_INCLUDE_DIR})
add_library(daemonxsd strigidaemon.cxx)
target_link_libraries(daemonxsd ${XERCESC_LIBRARIES})
