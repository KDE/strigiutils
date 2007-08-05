/* config.h. Generated by cmake from config.h.cmake */

#ifndef CONFIG_H
#define CONFIG_H

//////////////////////////////
// headers
//////////////////////////////
#cmakedefine HAVE_DIRECT_H 1
#cmakedefine HAVE_DIRENT_H 1
#cmakedefine HAVE_DLFCN_H 1
#cmakedefine HAVE_NDIR_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_SYS_DIR_H 1
#cmakedefine HAVE_SYS_NDIR_H 1
#cmakedefine HAVE_SYS_SOCKET_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_SOCKET_H 1
#cmakedefine HAVE_STDDEF_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine HAVE_WINDOWS_H 1

//////////////////////////////
// functions
//////////////////////////////
#cmakedefine HAVE_FCHDIR 1
#cmakedefine HAVE_GETTIMEOFDAY 1
#cmakedefine HAVE_ISBLANK 1
#cmakedefine HAVE_MKSTEMP 1
#cmakedefine HAVE_NANOSLEEP 1
#cmakedefine HAVE_STRCASECMP 1
#cmakedefine HAVE_STRCASESTR 1
#cmakedefine HAVE_STRLWR 1
#cmakedefine HAVE_STRNCASECMP 1

//////////////////////////////
//thread stuff
//////////////////////////////
#cmakedefine CMAKE_USE_WIN32_THREADS_INIT 1
#cmakedefine CMAKE_HAVE_PTHREAD_CREATE 1

//////////////////////////////
//types
//////////////////////////////
#cmakedefine HAVE_INT16_T 1
#cmakedefine HAVE_INT32_T 1
#cmakedefine HAVE_INT64_T 1
#cmakedefine HAVE_UINT 1
#cmakedefine HAVE_INTPTR_T 1
#cmakedefine HAVE_SIZE_T 1
#cmakedefine HAVE_SSIZE_T 1

//////////////////////////////
//missing functions
//////////////////////////////
#include "src/streams/strigi/compat.h"

//////////////////////////////
//misc
//////////////////////////////
#cmakedefine ICONV_SECOND_ARGUMENT_IS_CONST 1
#cmakedefine CMAKE_ANSI_FOR_SCOPE 1

#ifndef CMAKE_ANSI_FOR_SCOPE
 #define for if (0); else for
#endif

//////////////////////////////
//windows stuff
//////////////////////////////
#if defined(HAVE_WINDOWS_H) && !defined(__CYGWIN__)

 //need this for ChangeNotify and TryEnterCriticalSection
 //this wont compile for win98 though, but who cares?, not me :)
 #ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x400
 #endif

 #include <windows.h>
 #include <io.h>
 #if !defined(snprintf) && !defined(__MINGW32__)
    #define snprintf _snprintf
 #endif
#endif

#ifndef S_ISREG
    #define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
    #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_IRWXU
# define	S_IRWXU 	(S_IRUSR | S_IWUSR | S_IXUSR)
# define		S_IRUSR	0000400	/* read permission, owner */
# define		S_IWUSR	0000200	/* write permission, owner */
# define		S_IXUSR 0000100/* execute/search permission, owner */
#endif

#ifndef S_IRWXG
# define	S_IRWXG		(S_IRGRP | S_IWGRP | S_IXGRP)
# define		S_IRGRP	0000040	/* read permission, group */
# define		S_IWGRP	0000020	/* write permission, grougroup */
# define		S_IXGRP 0000010/* execute/search permission, group */
#endif

#ifndef S_IRWXO
# define	S_IRWXO		(S_IROTH | S_IWOTH | S_IXOTH)
# define		S_IROTH	0000004	/* read permission, other */
# define		S_IWOTH	0000002	/* write permission, other */
# define		S_IXOTH 0000001/* execute/search permission, other */
#endif

// set sleep time
#ifdef HAVE_NANOSLEEP
    #define strigi_nanosleep(nanoseconds) struct timespec sleeptime; sleeptime.tv_sec = 0; sleeptime.tv_nsec = nanoseconds; nanosleep(&sleeptime, 0);
#endif


#ifdef _MSC_VER
	#define sleep(x) Sleep(x*1000)
	
	#ifndef strigi_nanosleep
	    #define strigi_nanosleep(nanoseconds) Sleep(nanoseconds/1000000)
	#endif
#endif

#include <strigi/strigiconfig.h>

#define KDELIBSUFF "${KDELIBSUFF}"

#define LIBINSTALLDIR "${LIBINSTALLDIR}"

#define INSTALLDIR "${CMAKE_INSTALL_PREFIX}"

#define MIMEINSTALLDIR "${MIMEINSTALLDIR}"
#endif //CONFIG_H
