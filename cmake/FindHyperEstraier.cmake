#
# this module look for hyperestraier (http://hyperestraier.sourceforge.net/)
# EST_LIBS
# EST_CFLAGS
# EST_LDFLAGS
# EST_LIBDIR
# EST_INCLUDE_DIR
#

# find estconfig executable
FIND_PROGRAM(ESTCONFIG NAMES estconfig PATHS /bin )

# get configuration options
IF (ESTCONFIG)
  EXEC_PROGRAM(${ESTCONFIG} ARGS "--libs" OUTPUT_VARIABLE EST_LIBS)
  EXEC_PROGRAM(${ESTCONFIG} ARGS "--cflags" OUTPUT_VARIABLE EST_CFLAGS)
  EXEC_PROGRAM(${ESTCONFIG} ARGS "--ldflags" OUTPUT_VARIABLE EST_LDFLAGS)
  EXEC_PROGRAM(${ESTCONFIG} ARGS "--libdir" OUTPUT_VARIABLE EST_LIBDIR)
  EXEC_PROGRAM(${ESTCONFIG} ARGS "--headdir" OUTPUT_VARIABLE EST_INCLUDE_DIR)
  SET(HyperEstraier_FOUND 1 CACHE BOOL "Found Hyper Estraier library")
ENDIF (ESTCONFIG)

# print status message
IF(HyperEstraier_FOUND)
  IF(NOT HyperEstraier_FIND_QUIETLY)
    MESSAGE(STATUS "Found HyperEstraier: ${ESTCONFIG}")
  ENDIF(NOT HyperEstraier_FIND_QUIETLY)
ELSE(HyperEstraier_FOUND)
  IF(HyperEstraier_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find HyperEstraier. Please download HyperEstraier from http://hyperestraier.sf.net")
  ENDIF(HyperEstraier_FIND_REQUIRED)
ENDIF(HyperEstraier_FOUND)

