/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#ifndef HAVE_SETENV
int setenv(const char *name, const char *value, int overwrite)
{
    int i, iRet;
    char * a;

    if (!overwrite && getenv(name)) return 0;

    i = strlen(name) + strlen(value) + 2;
    a = (char*)malloc(i);
    if (!a) return 1;

    strcpy(a, name);
    strcat(a, "=");
    strcat(a, value);

    iRet = putenv(a);
    free(a);
    return iRet;
}
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char* sa, const char* sb){
    char ca,cb;
    if (sa == sb)
        return 0;
    int i=0;

    do{
        ca = tolower( (*(sa++)) );
        cb = tolower( (*(sb++)) );

        i++;
    } while ( ca != L'\0' && (ca == cb) );

    return (int)(ca - cb);
}
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char* sa, const char* sb, int l){
    char ca,cb;
    if (sa == sb)
        return 0;
    int i=0;

    do{
        if ( i >= l )
            break;

        ca = tolower( (*(sa++)) );
        cb = tolower( (*(sb++)) );

        i++;
    } while ( ca != L'\0' && (ca == cb) );

    return (int)(ca - cb);
}
#endif

#ifndef HAVE_STRCASESTR
const char * strcasestr(const char *big, const char *little){
    char* tmp1 = strdup(big);
    char* tmp2 = strdup(little);
#ifdef HAVE_STRLWR /* for windows */
    strlwr(tmp1);
    strlwr(tmp2);
#else /* for solaris */
    char* t = tmp1;
    while (*t) {
        tolower(*t);
        ++t;
    }
    t = tmp2;
    while (*t) {
        tolower(*t);
        ++t;
    }
#endif

    const char * ret = strstr(tmp1,tmp2);

    if ( ret != NULL ){
        ret = big + (ret-tmp1);
    }

    free(tmp1);
    free(tmp2);

    return ret;
}
#endif

#ifndef HAVE_ISBLANK
int isblank(char c){
    if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
     return 1;

    return 0;
}
#endif

#ifndef HAVE_MKSTEMP
#ifdef _WIN32
 #include <fcntl.h>
 #include <sys/stat.h>
#endif

int mkstemp(char *tmpl)
{
   mktemp(tmpl);
   return open(tmpl,O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
}
#endif
