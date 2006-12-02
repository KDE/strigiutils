#
# this module look for expat support
# it will define the following values
#
# EXPAT_INCLUDE_DIR  = where expat.h can be found
# EXPAT_LIBRARY      = the library to link against libxml2
# FOUND_EXPAT        = set to 1 if libexpat is found
#
IF(EXISTS ${PROJECT_CMAKE}/ExpatConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/ExpatConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/ExpatConfig.cmake)

IF(Expat_INCLUDE_DIRS)

  FIND_PATH(EXPAT_INCLUDE_DIR expat.h ${Expat_INCLUDE_DIRS})
  FIND_LIBRARY(EXPAT_LIBRARY expat ${Expat_LIBRARY_DIRS})

ELSE(Expat_INCLUDE_DIRS)

  SET(TRIAL_LIBRARY_PATHS
    $ENV{EXPAT_HOME}${LIB_DESTINATION}
    /usr${LIB_DESTINATION}
    /usr/local${LIB_DESTINATION}
    /sw${LIB_DESTINATION}
  ) 
  SET(TRIAL_INCLUDE_PATHS
    $ENV{EXPAT_HOME}/include
    /usr/include
    /usr/local/include
    /sw/include
  ) 

  FIND_LIBRARY(EXPAT_LIBRARY expat ${TRIAL_LIBRARY_PATHS})
  FIND_PATH(EXPAT_INCLUDE_DIR expat.h ${TRIAL_INCLUDE_PATHS})

ENDIF(Expat_INCLUDE_DIRS)

IF(EXPAT_INCLUDE_DIR AND EXPAT_LIBRARY)
  SET(Expat_FOUND 1 CACHE BOOL "Found expat library")
ELSE(EXPAT_INCLUDE_DIR AND EXPAT_LIBRARY)
  SET(Expat_FOUND 0 CACHE BOOL "Not found expat library")
ENDIF(EXPAT_INCLUDE_DIR AND EXPAT_LIBRARY)

