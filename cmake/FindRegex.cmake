# - Try to find the libregex libraries
# Once done this will define
#
# Regex_FOUND - system has libregex
# REGEX_INCLUDE_DIR - the libregex include directory
# REGEX_LIBRARIES - libregex library
FIND_PATH(REGEX_INCLUDE_DIR regex.h /usr/include /usr/local/include )
FIND_LIBRARY(REGEX_LIBRARIES NAMES libregex PATHS /usr/lib /usr/local/lib )
if(REGEX_INCLUDE_DIR AND REGEX_LIBRARIES)
 set(Regex_FOUND TRUE)
endif(REGEX_INCLUDE_DIR AND REGEX_LIBRARIES)

IF(REGEX_INCLUDE_DIR AND REGEX_LIBRARIES)
  SET(Regex_FOUND 1)
  if(NOT Regex_FIND_QUIETLY)
   message(STATUS "Found libregex: ${REGEX_LIBRARIES}")
  endif(NOT Regex_FIND_QUIETLY)
ELSE(REGEX_INCLUDE_DIR AND REGEX_LIBRARIES)
  SET(Regex_FOUND 0 CACHE BOOL "Not found regex library")
ENDIF(REGEX_INCLUDE_DIR AND REGEX_LIBRARIES)

MARK_AS_ADVANCED(Regex_FOUND REGEX_INCLUDE_DIR REGEX_LIBRARIES)
