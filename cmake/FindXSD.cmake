# Locate Xsd from code synthesis include paths and binary
# Xsd can be found at http://codesynthesis.com/products/xsd/
# Written by Frederic Heem, frederic.heem _at_ telsey.it
# Modified by Jos van den Oever

# This module defines
# XSD_INCLUDE_DIR, where to find elements.hxx, etc.
# XSD_EXECUTABLE, where is the xsd compiler
# XSD_FOUND, If false, don't try to use xsd

FIND_PATH(XSD_INCLUDE_DIR cxx/parser/elements.hxx
  "[HKEY_CURRENT_USER\\software\\xsd\\include]"
  "[HKEY_CURRENT_USER]\\xsd\\include]"
  $ENV{XSDDIR}/include
  /usr/local/include/xsd
  /usr/include/xsd
)

FIND_PROGRAM(XSD_EXECUTABLE 
  NAMES 
    xsd
  PATHS
#    "[HKEY_CURRENT_USER\\xsd\\bin"
    $ENV{XSDDIR}/bin 
    /usr/local/bin
    /usr/bin
)

# if the include a the library are found then we have it
IF(XSD_INCLUDE_DIR AND XSD_EXECUTABLE)
  SET(XSD_FOUND "YES" )
  IF(NOT XSD_FIND_QUIETLY)
    MESSAGE(STATUS "Found xsd: ${XSD_EXECUTABLE}")
  ENDIF(NOT XSD_FIND_QUIETLY)
ELSE(XSD_INCLUDE_DIR AND XSD_EXECUTABLE)
  IF(XSD_FIND_REQUIRED)
    MESSAGE(FATAL "xsd was not found.")
  ENDIF(XSD_FIND_REQUIRED)
  IF(NOT XSD_FIND_QUIETLY)
    MESSAGE(STATUS "xsd was not found.")
  ENDIF(NOT XSD_FIND_QUIETLY)
ENDIF(XSD_INCLUDE_DIR AND XSD_EXECUTABLE)

MARK_AS_ADVANCED(
  XSD_INCLUDE_DIR
  XSD_EXECUTABLE
) 
