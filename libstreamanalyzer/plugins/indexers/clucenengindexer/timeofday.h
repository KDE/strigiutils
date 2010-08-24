/*
* Defines gettimeofday
*
* Based on timeval.h Copyright (c) by Wu Yongwei <wuyongwei@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "config.h"
#include <errno.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
 #define EPOCHFILETIME (116444736000000000i64)
#else
 #define EPOCHFILETIME (116444736000000000LL)
#endif

#ifndef HAVE_GETTIMEOFDAY
__inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
#ifdef _WINDOWS
    FILETIME        ft;
    LARGE_INTEGER   li;
    __int64         t;
    static int      tzflag;

    if (tv)
    {
        GetSystemTimeAsFileTime(&ft);
        li.LowPart  = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart;       /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;     /* Offset to the Epoch time */
        t /= 10;                /* In microseconds */
        tv->tv_sec  = (long)(t / 1000000);
        tv->tv_usec = (long)(t % 1000000);
    }

    if (tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
    }

    return 0;

#else /* !defined(_WINDOWS) */
#error No gettimeofday function was found.
#endif /* _WINDOWS */
}
#else /* HAVE_GETTIMEOFDAY */
    #include "sys/time.h" //linux defines gettimeofday here...
#endif /* HAVE_GETTIMEOFDAY */


