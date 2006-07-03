#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene
# CLUCENE_FOUND        = set to 1 if clucene is found
#
IF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/CLuceneConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)

IF(CLucene_INCLUDE_DIRS)

  FIND_PATH(CLUCENE_INCLUDE_DIR CLucene/StdHeader.h ${CLucene_INCLUDE_DIRS})
  FIND_LIBRARY(CLUCENE_LIBRARY xml2 ${CLucene_LIBRARY_DIRS})

ELSE(CLucene_INCLUDE_DIRS)

  SET(TRIAL_LIBRARY_PATHS
    $ENV{CLUCENE_HOME}/lib
    /usr/lib
    /usr/local/lib
    /sw/lib
  ) 
  SET(TRIAL_INCLUDE_PATHS
    $ENV{CLUCENE_HOME}/include
    /usr/include
    /usr/local/include
    /sw/include
  ) 

  FIND_LIBRARY(CLUCENE_LIBRARY clucene ${TRIAL_LIBRARY_PATHS})
  FIND_PATH(CLUCENE_INCLUDE_DIR CLucene/StdHeader.h ${TRIAL_INCLUDE_PATHS})
  FIND_PATH(CLUCENE_LIBRARY_DIR CLucene/clucene-config.h ${TRIAL_LIBRARY_PATHS})

ENDIF(CLucene_INCLUDE_DIRS)

IF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY)
  SET(CLUCENE_FOUND 1 CACHE BOOL "Found CLucene library")
ELSE(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY)
  SET(CLUCENE_FOUND 0 CACHE BOOL "Not found CLucene library")
ENDIF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY)

IF(CLUCENE_FOUND)
  IF(NOT CLUCENE_FIND_QUIETLY)
    MESSAGE(STATUS "Found CLucene: ${CLUCEN_LIBRARY}")
  ENDIF(NOT CLUCENE_FIND_QUIETLY)
ELSE(CLUCENE_FOUND)
  IF(CLucene_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CLucene")
  ENDIF(CLucene_FIND_REQUIRED)
ENDIF(CLUCENE_FOUND)

MARK_AS_ADVANCED(
  CLUCENE_INCLUDE_DIR 
  CLUCENE_LIBRARY_DIR 
  CLUCENE_LIBRARY 
)
