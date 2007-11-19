# - Try to find the fam libraries
# Once done this will define
#
# FAM_FOUND - system supports fam
# FAM_INCLUDE_DIR - the fam include directory
# FAM_LIBRARIES - libfam library

FIND_PATH(FAM_INCLUDE_DIR fam.h PATHS /usr/include /usr/local/include )
FIND_LIBRARY(FAM_LIBRARIES NAMES fam )

IF(FAM_INCLUDE_DIR AND FAM_LIBRARIES)
  SET(FAM_FOUND 1)
  if(NOT FAM_FIND_QUIETLY)
   message(STATUS "Found FAM: ${FAM_LIBRARIES}")
  endif(NOT FAM_FIND_QUIETLY)
ELSE(FAM_INCLUDE_DIR AND FAM_LIBRARIES)
  SET(FAM_FOUND 0 CACHE BOOL "Not found FAM")
  message(STATUS "NOT Found FAM, disabling it")
ENDIF(FAM_INCLUDE_DIR AND FAM_LIBRARIES)

MARK_AS_ADVANCED(FAM_INCLUDE_DIR FAM_LIBRARIES)

