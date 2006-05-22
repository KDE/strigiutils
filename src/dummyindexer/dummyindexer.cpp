#include "indexer.h"

int
main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: %s [dir-to-index]\n", argv[0]);
		return -1;
	}
        Indexer indexer;
	indexer.index(argv[1]);
	return 0;
}
