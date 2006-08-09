#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene
# CLucene_FOUND        = set to 1 if clucene is found
#
 INCLUDE(CheckSymbolExists)

IF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/CLuceneConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)

  SET(TRIAL_LIBRARY_PATHS
    $ENV{CLUCENE_HOME}/lib
    $ENV{HOME}/testinstall/lib
    PATHS
    /usr/lib
    /usr/local/lib
    /sw/lib
  ) 
  SET(TRIAL_INCLUDE_PATHS
    $ENV{CLUCENE_HOME}/include
    $ENV{HOME}/testinstall/include
    PATHS
    /usr/include
    /usr/local/include
    /sw/include
  ) 

  FIND_LIBRARY(CLUCENE_LIBRARY clucene NAMES clucene-core ${TRIAL_LIBRARY_PATHS})
  IF (CLUCENE_LIBRARY)
    MESSAGE(STATUS "Found CLucene library: ${CLUCENE_LIBRARY}")
  ENDIF (CLUCENE_LIBRARY)
  FIND_PATH(CLUCENE_INCLUDE_DIR CLucene.h ${TRIAL_INCLUDE_PATHS})

  # check if version 0.9.15 is patched
  IF (EXISTS ${CLUCENE_INCLUDE_DIR}/CLucene/util/streambase.h)
    FILE(READ ${CLUCENE_INCLUDE_DIR}/CLucene/util/streambase.h CLCONTENT)
    STRING(REGEX MATCH "Apache" CLMATCH ${CLCONTENT})
    IF (CLMATCH)
      MESSAGE(FATAL_ERROR " CLucene 0.9.15 was not patched. Please read replace the files\n     streambase.h, bufferedstream.h, and inputstreambuffer.h\n in CLucene with those from Strigi. (See README)")
    ENDIF (CLMATCH)
  ELSE (EXISTS ${CLUCENE_INCLUDE_DIR}/CLucene/util/streambase.h)
      MESSAGE(FATAL_ERROR " CLucene 0.9.15 was not patched. Please read replace the files\n     streambase.h, bufferedstream.h, and inputstreambuffer.h\n in CLucene with those from Strigi. (See README)")
  ENDIF (EXISTS ${CLUCENE_INCLUDE_DIR}/CLucene/util/streambase.h)

  # end check

  IF (CLUCENE_INCLUDE_DIR)
    MESSAGE(STATUS "Found CLucene include dir: ${CLUCENE_INCLUDE_DIR}")
  ENDIF (CLUCENE_INCLUDE_DIR)
  
  IF(NOT MSVC)
    FIND_PATH(CLUCENE_LIBRARY_DIR CLucene/clucene-config.h ${TRIAL_LIBRARY_PATHS})
    IF (CLUCENE_LIBRARY_DIR)
      MESSAGE(STATUS "Found CLucene library dir: ${CLUCENE_LIBRARY_DIR}")
      FILE(READ ${CLUCENE_LIBRARY_DIR}/CLucene/clucene-config.h CLCONTENT)
      STRING(REGEX MATCH "_CL_VERSION +\".*\"" CLMATCH ${CLCONTENT})
      IF (CLMATCH)
        STRING(REGEX REPLACE "_CL_VERSION +\"(.*)\"" "\\1" CLVERSION ${CLMATCH})
      ENDIF (CLMATCH)
      IF (CLVERSION STRLESS "0.9.15")
        MESSAGE(FATAL_ERROR "CLucene version is less than 0.9.15")
      ENDIF (CLVERSION STRLESS "0.9.15")
    ENDIF (CLUCENE_LIBRARY_DIR)
  ELSE(NOT MSVC)
    #msvc doesnt use a config file
    SET(CLUCENE_LIBRARY_DIR ${CLUCENE_INCLUDE_DIR})
  ENDIF(NOT MSVC)


IF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)
  SET(CLucene_FOUND 1)
  IF(NOT CLucene_FIND_QUIETLY)
    MESSAGE(STATUS "Found CLucene: ${CLUCENE_LIBRARY}")
  ENDIF(NOT CLucene_FIND_QUIETLY)
ELSE(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)
  SET(CLucene_FOUND 0)
  IF(CLucene_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CLucene. Please download CLucene from http://clucene.sf.net")
  ENDIF(CLucene_FIND_REQUIRED)
ENDIF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)

MARK_AS_ADVANCED(
  CLUCENE_INCLUDE_DIR 
  CLUCENE_LIBRARY_DIR 
  CLUCENE_LIBRARY 
)
