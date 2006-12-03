
# create a library with the shared code for parsing xml files
include_directories(xsdparser)
add_library(xmlstream xsdparser/xmlstream.cpp)
target_link_libraries(xmlstream expat)

# java is required to compile the xsd parser
find_package(Java REQUIRED)

# loop over all xsd files, the next line does not work everywhere,
# so we dont use it currently
# file(GLOB XSDFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.xsd)
# if the previous line does not work, try this:
set(XSDFILES "strigidaemonconfiguration.xsd")

foreach (XSDFILE ${XSDFILES})
  string(REPLACE ".xsd" "" XSDNAME ${XSDFILE})

  # generate the io code from the XML Schema file
  add_custom_command(
	OUTPUT ${XSDNAME}.cpp
		${XSDNAME}test.cpp
  	COMMAND ${JAVA_COMPILE} -d ${CMAKE_CURRENT_BINARY_DIR}
		xsdparser/xsdparser.java
  	COMMAND ${JAVA_RUNTIME} -cp ${CMAKE_CURRENT_BINARY_DIR}
  		xsdparser.xsdparser ${XSDFILE}
  	DEPENDS xsdparser/xsdparser.java ${XSDFILE}
  	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  # create a library from the xml io files
  add_library(${XSDNAME} ${XSDNAME}.cpp)
  target_link_libraries(${XSDNAME} xmlstream)

  # generate a test executable
  add_executable(${XSDNAME}test ${XSDNAME}test.cpp)
  target_link_libraries(${XSDNAME}test ${XSDNAME} xmlstream)

endforeach (XSDFILE ${XSDFILES})

# create a library from the xml io files
add_library(daemonconfigurator daemonconfigurator.cpp)
target_link_libraries( daemonconfigurator ${XSDNAME})
