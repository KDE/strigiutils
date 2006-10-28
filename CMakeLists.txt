PROJECT (strigi)
set(STRIGI_VERSION_MAJOR 0)
set(STRIGI_VERSION_MINOR 3)
set(STRIGI_VERSION_PATCH 8)

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


IF(NOT MSVC)
    add_definitions (-fPIC)
    set(CMAKE_CXX_FLAGS "-Wall -g3 -O0 -ansi")
ELSE(NOT MSVC)

ENDIF(NOT MSVC)

OPTION(ENABLE_INOTIFY
  "enable inotify support"
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
find_package(CLucene REQUIRED)
find_package(Iconv REQUIRED)
find_package(Expat REQUIRED)
# Xerces requirement switched off until the next release
#find_package(XERCESC REQUIRED)
find_package(Magic REQUIRED)
find_package(XSD)
find_package(HyperEstraier)
find_package(SQLite)
find_package(Libxml2)
find_package(Qt4)
find_package(XAttr)
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


#need to find a few default headers:
INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE_CXX(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE_CXX(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE_CXX(socket.h HAVE_SOCKET_H)
CHECK_INCLUDE_FILE_CXX(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILE_CXX(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CXX(sys/ndir.h HAVE_SYS_NDIR_H)
CHECK_INCLUDE_FILE_CXX(sys/dir.h HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILE_CXX(ndir.h HAVE_NDIR_H)
CHECK_INCLUDE_FILE_CXX(dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)
CHECK_INCLUDE_FILE_CXX(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILE_CXX(direct.h HAVE_DIRECT_H)

#test for ansi scope (needed for msvc6)
INCLUDE(TestForANSIForScope)

#test for some functions that are missing on windows
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
CHECK_FUNCTION_EXISTS(isblank HAVE_ISBLANK)
CHECK_FUNCTION_EXISTS(fchdir HAVE_FCHDIR)

#test for missing types
INCLUDE(CheckTypeSize)

SET(CHECK_TYPE_SIZE_PREINCLUDE "#include <sys/socket.h>")
SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES} stdint.h")
CHECK_TYPE_SIZE(int64_t INT64_T)
CHECK_TYPE_SIZE(__int64 __INT64)
CHECK_TYPE_SIZE("long long" SIZEOF_LONGLONG)

CHECK_TYPE_SIZE(int32_t INT32_T)
CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(ssize_t SIZEOF_SSIZE_T)

CHECK_TYPE_SIZE(uint UINT)
#CHECK_SYMBOL_EXISTS(socklen_t sys/socket.h socket.h SOCKLEN_T)
CHECK_TYPE_SIZE(intptr_t INTPTR_T)

#now write out our configuration....
CONFIGURE_FILE(${strigi_SOURCE_DIR}/src/streams/jstreamsconfig.h.cmake
  ${strigi_SOURCE_DIR}/src/streams/jstreamsconfig.h)

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
#SET(CPACK_PACKAGE_EXECUTABLES "strigi" "strigi")
INCLUDE(CPack)
