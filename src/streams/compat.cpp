#include "jstreamsconfig.h"

#include <string.h>
#include <stdlib.h>


#ifndef HAVE_STRCASESTR
int strncasecmp(const char* a, const char* b, int l){
	char* tmp1=strdup(a);
	char* tmp2=strdup(b);

	strlwr(tmp1);
	strlwr(tmp2);

	int ret = strncmp(tmp1,tmp2,l);

	free(tmp1);
	free(tmp2);

	return ret;
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
