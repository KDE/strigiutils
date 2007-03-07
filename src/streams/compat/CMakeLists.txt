
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
CHECK_INCLUDE_FILE_CXX(stdint.h HAVE_STDINT_H)

#test for ansi scope (needed for msvc6)
INCLUDE(TestForANSIForScope)

#test for some functions that are missing on a particular system
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
CHECK_FUNCTION_EXISTS(isblank HAVE_ISBLANK)
CHECK_FUNCTION_EXISTS(fchdir HAVE_FCHDIR)
CHECK_FUNCTION_EXISTS(strlwr HAVE_STRLWR)
CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)

#test for missing types
INCLUDE(CheckTypeSize)

SET(CHECK_TYPE_SIZE_PREINCLUDE "#include <sys/socket.h>")
SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES} stdint.h")
CHECK_TYPE_SIZE(int64_t INT64_T)
CHECK_TYPE_SIZE(__int64 __INT64)
CHECK_TYPE_SIZE("long long" SIZEOF_LONGLONG)
IF( ${SIZEOF_LONGLONG} STREQUAL "" )
 set( SIZEOF_LONGLONG "0")
ENDIF( ${SIZEOF_LONGLONG} STREQUAL "" )

CHECK_TYPE_SIZE(int32_t INT32_T)
CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(ssize_t SIZEOF_SSIZE_T)

CHECK_TYPE_SIZE(uint UINT)
#CHECK_SYMBOL_EXISTS(socklen_t sys/socket.h socket.h SOCKLEN_T)
CHECK_TYPE_SIZE(intptr_t INTPTR_T)

#now write out our configuration....
CONFIGURE_FILE(${strigi_SOURCE_DIR}/src/streams/compat/jstreamsconfig.h.cmake
  ${strigi_BINARY_DIR}/src/streams/compat/jstreamsconfig.h)

CONFIGURE_FILE(
  ${strigi_SOURCE_DIR}/src/streams/compat/strigi_thread_config.h.cmake
  ${strigi_BINARY_DIR}/src/streams/compat/strigi_thread_config.h)
