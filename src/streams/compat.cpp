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
#include "jstreamsconfig.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#ifndef HAVE_STRCASESTR
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
bool isblank(char c){
    if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
     return true;

    return false;
}
#endif
