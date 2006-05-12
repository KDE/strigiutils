#include "indexer.h"

int
main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s [indexdir] [dir-to-index]\n", argv[0]);
		return -1;
	}
        Indexer indexer(argv[1]);
	try {
		indexer.index(argv[2]);
	} catch (...) {
		printf("hmm\n");
	}
	return 0;
}
