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
    char* tmp1=strdup(big);
    char* tmp2=strdup(little);
    strlwr(tmp1);
    strlwr(tmp2);

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
