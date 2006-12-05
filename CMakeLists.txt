PROJECT (strigi)
set(STRIGI_VERSION_MAJOR 0)
set(STRIGI_VERSION_MINOR 3)
set(STRIGI_VERSION_PATCH 10)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.2 FATAL_ERROR)

# include specific modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include general modules
include(UsePkgConfig)

# compile in debug mode
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#add extra search paths for libraries and includes
SET ( STRIGI_EXTRA_INC_DIRECTORY "" CACHE STRING "Extra include directories to search in")
SET ( STRIGI_EXTRA_LIB_DIRECTORY "" CACHE STRING "Extra library directories to search in" )
SET (CMAKE_INCLUDE_PATH "${STRIGI_EXTRA_INC_DIRECTORY};${CMAKE_INCLUDE_PATH}")
SET (CMAKE_LIBRARY_PATH "${STRIGI_EXTRA_LIB_DIRECTORY};${CMAKE_LIBRARY_PATH}")
SET (LIB_DESTINATION "lib" CACHE STRING "Library directory suffix")

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ELSE(MSVC)
    add_definitions (-fPIC)
    set(CMAKE_CXX_FLAGS "-Wall -g3 -O0 -ansi")
ENDIF(MSVC)

IF(CMAKE_SYSTEM MATCHES "SunOS-5*.")
    remove_definitions(-fPIC)
    set(CMAKE_CXX_FLAGS "")
ENDIF(CMAKE_SYSTEM MATCHES "SunOS-5*.")

OPTION(ENABLE_INOTIFY
  "enable inotify support (unstable)"
  OFF)

OPTION(ENABLE_DBUS
  "enable dbus support"
  ON)

OPTION(ENABLE_LOG4CXX
  "enable log4cxx support for advanced logging"
  OFF)

SUBDIRS (src)
ENABLE_TESTING()

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Sha REQUIRED)
find_package(Threads REQUIRED)
find_package(Iconv REQUIRED)
find_package(Expat REQUIRED)
# Xerces requirement switched off until the next release
find_package(CLucene)
find_package(XERCESC)
find_package(Magic REQUIRED)
find_package(HyperEstraier)
find_package(SQLite)
find_package(Libxml2)
find_package(XAttr)
set(QT_MIN_VERSION "4.2.0")
find_package(Qt4)
if (NOT QT4_FOUND)
    MESSAGE(STATUS "Qt4 was not found. No GUI will be built.")
endif (NOT QT4_FOUND)

IF(ENABLE_DBUS)
  include(UsePkgConfig)
  PKGCONFIG(dbus-1 DBUS_INCLUDE_DIR DBUS_LIBRARY_DIR DBUS_LDFLAGS DBUS_CFLAGS)
  if (NOT DBUS_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "Could not find DBus")
  endif (NOT DBUS_INCLUDE_DIR)
ENDIF(ENABLE_DBUS)

if (ENABLE_LOG4CXX)
    find_package (Log4cxx)
endif (ENABLE_LOG4CXX)


CONFIGURE_FILE(
  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# packaging target
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Strigi Desktop Search")
set(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR ${STRIGI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${STRIGI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${STRIGI_VERSION_PATCH})
set(STRIGI_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
INCLUDE(CPack)
INCLUDE(Dart)
