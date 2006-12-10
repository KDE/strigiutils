#include "base64inputstream.h"
#include <stdio.h>
using namespace jstreams;
using namespace std;

int
main(int argc, char** argv) {
   for (int i=1; i<argc; ++i) {
       string out = Base64InputStream::decode(argv[i], strlen(argv[i]));
       printf("%s\n", out.c_str());
   }
   return 0;
}
