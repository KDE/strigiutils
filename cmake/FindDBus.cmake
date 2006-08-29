#
# this module look for dbus support
# it will define the following values
#
# DBus_INCLUDE_DIR  = where dbus.h can be found
# DBus_LIBRARY      = the library to link against libdbus-1
# FOUND_DBus        = set to 1 if libdbus_1 is found
#
IF(EXISTS ${PROJECT_CMAKE}/DBusConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/DBusConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/DBusConfig.cmake)

IF(DBus_INCLUDE_DIRS)

  FIND_PATH(DBus_INCLUDE_DIR dbus.h ${DBus_INCLUDE_DIRS})
  FIND_LIBRARY(DBus_LIBRARY dbus ${DBus_LIBRARY_DIRS})

ELSE(DBus_INCLUDE_DIRS)

  SET(TRIAL_LIBRARY_PATHS
    $ENV{DBus_HOME}/lib
    /usr/lib
    /usr/local/lib
    /sw/lib
  ) 
  SET(TRIAL_LIBRARY_INCLUDE_PATHS
    $ENV{DBus_HOME}/lib/dbus-1.0/include
    /usr/lib/dbus-1.0/include
    /usr/local/lib/dbus-1.0/include
    /sw/lib/dbus-1.0/include
  ) 
  SET(TRIAL_INCLUDE_PATHS
    $ENV{DBus_HOME}/include/dbus-1.0
    /usr/include/dbus-1.0
    /usr/local/include/dbus-1.0
    /sw/include/dbus-1.0
  ) 

  FIND_LIBRARY(DBus_LIBRARY dbus-1 ${TRIAL_LIBRARY_PATHS})
  FIND_PATH(DBus_LIBRARY_DIR dbus/dbus-arch-deps.h
    ${TRIAL_LIBRARY_INCLUDE_PATHS})
  FIND_PATH(DBus_INCLUDE_DIR dbus/dbus.h ${TRIAL_INCLUDE_PATHS})
  SET(DBus_INCLUDE_DIR ${DBus_LIBRARY_DIR} ${DBus_INCLUDE_DIR})

ENDIF(DBus_INCLUDE_DIRS)

IF(DBus_INCLUDE_DIR AND DBus_LIBRARY AND DBus_LIBRARY_DIR)
  SET(DBus_FOUND 1 CACHE BOOL "Found dbus library")
ELSE(DBus_INCLUDE_DIR AND DBus_LIBRARY AND DBus_LIBRARY_DIR)
  SET(DBus_FOUND 0 CACHE BOOL "Not found dbus library")
  IF(DBus_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find DBus")
  ENDIF(DBus_FIND_REQUIRED)
ENDIF(DBus_INCLUDE_DIR AND DBus_LIBRARY AND DBus_LIBRARY_DIR)

