# - Try to find the libmagic libraries
# Once done this will define
#
# Magic_FOUND - system has libmagic
# MAGIC_INCLUDE_DIR - the libmagic include directory
# MAGIC_LIBRARIES - libmagic library
FIND_PATH(MAGIC_INCLUDE_DIR magic.h /usr/include /usr/local/include )
FIND_LIBRARY(MAGIC_LIBRARIES NAMES magic PATHS /usr/lib /usr/local/lib )
if(MAGIC_INCLUDE_DIR AND MAGIC_LIBRARIES)
 set(Magic_FOUND TRUE)
endif(MAGIC_INCLUDE_DIR AND MAGIC_LIBRARIES)

IF(MAGIC_INCLUDE_DIR AND MAGIC_LIBRARIES)
  SET(Magic_FOUND 1)
  if(NOT Magic_FIND_QUIETLY)
   message(STATUS "Found libmagic: ${MAGIC_LIBRARIES}")
  endif(NOT Magic_FIND_QUIETLY)
ELSE(MAGIC_INCLUDE_DIR AND MAGIC_LIBRARIES)
  SET(Magic_FOUND 0 CACHE BOOL "Not found magic library")
ENDIF(MAGIC_INCLUDE_DIR AND MAGIC_LIBRARIES)

MARK_AS_ADVANCED(MAGIC_INCLUDE_DIR MAGIC_LIBRARIES)
