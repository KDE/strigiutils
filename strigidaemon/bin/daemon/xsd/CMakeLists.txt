# create a library with the shared code for parsing xml files

if (LIBXML2_FOUND)
  add_library(xmlstream STATIC
	xsdparser/libxmlstream.cpp
	xsdparser/xmlstream.cpp
  )
  target_link_libraries(xmlstream ${LIBXML2_LIBRARIES})
endif (LIBXML2_FOUND)

# javac >= 1.5 is required to compile the xsd parser
OPTION(ENABLE_REGENERATEXSD
  "regenerate xsd parser from *.xsd files(not normally required)" ON)
if(ENABLE_REGENERATEXSD)
find_package(Java)
if (JAVA_COMPILE)
  execute_process(COMMAND ${JAVA_COMPILE} "-version"
    OUTPUT_VARIABLE JAVAC_VERSION
    ERROR_VARIABLE JAVAC_VERSION)
  if (JAVAC_VERSION MATCHES " 1\\.[56]" AND NOT JAVAC_VERSION MATCHES "g[ci]j")
    set(JAVAC5_FOUND TRUE)
  endif (JAVAC_VERSION MATCHES " 1\\.[56]" AND NOT JAVAC_VERSION MATCHES "g[ci]j")
  # test that java runtime is not gij either
  execute_process(COMMAND ${JAVA_RUNTIME} "-version"
    OUTPUT_VARIABLE JAVA_VERSION
    ERROR_VARIABLE JAVA_VERSION)
  if (JAVA_VERSION MATCHES "g[ci]j")
    set(JAVAC5_FOUND FALSE)
  endif (JAVA_VERSION MATCHES "g[ci]j")
  MESSAGE("Java5 found ${JAVAC5_FOUND}")
endif (JAVA_COMPILE)
endif(ENABLE_REGENERATEXSD)

# loop over all xsd files
set(XSDFILES "${CMAKE_CURRENT_SOURCE_DIR}/strigidaemonconfiguration.xsd" 
             "${CMAKE_CURRENT_SOURCE_DIR}/metadataproperties.xsd")

foreach (XSDFILE ${XSDFILES})
  get_filename_component(XSDNAME "${XSDFILE}" NAME_WE)

  # generate the io code from the XML Schema file
  SET(CPPFILE "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.cpp")

  if (JAVAC5_FOUND AND NOT WIN32 AND ENABLE_REGENERATEXSD)
    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.cpp"
             "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.h"
             "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}test.cpp"
      COMMAND ${JAVA_COMPILE} -d ${CMAKE_CURRENT_BINARY_DIR}
             "${CMAKE_CURRENT_SOURCE_DIR}/xsdparser/xsdparser.java"
      COMMAND ${JAVA_RUNTIME} -cp ${CMAKE_CURRENT_BINARY_DIR}
			 xsdparser.xsdparser ${XSDFILE}
      DEPENDS xsdparser/xsdparser.java ${XSDFILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  else (JAVAC5_FOUND AND NOT WIN32 AND ENABLE_REGENERATEXSD)
    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.cpp"
             "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.h"
             "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}test.cpp"
      COMMAND ${CMAKE_COMMAND}
        ARGS  -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${XSDNAME}.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.cpp"
      COMMAND ${CMAKE_COMMAND}
        ARGS  -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${XSDNAME}.h"
                "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}.h"
      COMMAND ${CMAKE_COMMAND}
        ARGS  -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${XSDNAME}test.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${XSDNAME}test.cpp"
    )
  endif (JAVAC5_FOUND AND NOT WIN32 AND ENABLE_REGENERATEXSD)

  # create a library from the xml io files
  add_library(${XSDNAME} STATIC ${CPPFILE})
  target_link_libraries(${XSDNAME} xmlstream)

  # generate a test executable
  add_executable(${XSDNAME}test ${XSDNAME}test.cpp)
  target_link_libraries(${XSDNAME}test ${XSDNAME} xmlstream)

endforeach (XSDFILE ${XSDFILES})
