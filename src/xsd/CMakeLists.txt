
# create a library with the shared code for parsing xml files
include_directories(xsdparser ${ICONV_INCLUDE_DIR})

if (LIBXML2_FOUND)
  include_directories(${LIBXML2_INCLUDE_DIR})
  add_library(xmlstream
	xsdparser/libxmlstream.cpp
	xsdparser/xmlstream.cpp
  )
  target_link_libraries(xmlstream ${LIBXML2_LIBRARIES})
else (LIBXML2_FOUND)
  # we use expat
  include_directories(${EXPAT_INCLUDE_DIR})
  add_library(xmlstream
	xsdparser/expatxmlstream.cpp
	xsdparser/xmlstream.cpp
  )
  target_link_libraries(xmlstream ${EXPAT_LIBRARY})
endif (LIBXML2_FOUND)

# java is required to compile the xsd parser
OPTION(ENABLE_REGENERATEXSD "regenerate xsd parser from *.xsd files(not normally required)" ON)
FIND_OPTIONAL_DEP(Java ENABLE_REGENERATEXSD JAVA_COMPILE "generating xsd parser from *.xsd files")

# loop over all xsd files, the next line does not work everywhere,
# so we dont use it currently
# file(GLOB XSDFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.xsd)
# if the previous line does not work, try this:
set(XSDFILES strigidaemonconfiguration.xsd metadataproperties.xsd)

foreach (XSDFILE ${XSDFILES})
  string(REPLACE ".xsd" "" XSDNAME ${XSDFILE})

  # generate the io code from the XML Schema file
  SET(CPPFILE ${CMAKE_CURRENT_SOURCE_DIR}/${XSDNAME}.cpp)

  if (JAVA_COMPILE
        AND ${CMAKE_CURRENT_SOURCE_DIR}/${XSDFILE} IS_NEWER_THAN ${CPPFILE} )
    add_custom_command(
      OUTPUT ${XSDNAME}.cpp
        ${XSDNAME}test.cpp
      COMMAND ${JAVA_COMPILE} -d ${CMAKE_CURRENT_BINARY_DIR}
        xsdparser/xsdparser.java
      COMMAND ${JAVA_RUNTIME} -cp ${CMAKE_CURRENT_BINARY_DIR}
        xsdparser.xsdparser ${XSDFILE}
    DEPENDS xsdparser/xsdparser.java ${XSDFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif (JAVA_COMPILE
        AND ${CMAKE_CURRENT_SOURCE_DIR}/${XSDFILE} IS_NEWER_THAN ${CPPFILE} )

  # create a library from the xml io files
  add_library(${XSDNAME} ${XSDNAME}.cpp)
  target_link_libraries(${XSDNAME} xmlstream)

  # generate a test executable
  add_executable(${XSDNAME}test ${XSDNAME}test.cpp)
  target_link_libraries(${XSDNAME}test ${XSDNAME} xmlstream)

endforeach (XSDFILE ${XSDFILES})
