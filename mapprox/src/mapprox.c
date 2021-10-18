#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "class1.h"

FILE **files;
size_t no_files;
void *charm;
size_t deg;
size_t len;

int main(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Usage: mapprox <degree> <no_chars> [FILE...]\n");
		return 0;
	}

	srand(time(NULL));

	/* Initialize data */
	deg = atol(argv[1]);
	len = atol(argv[2]);

	if (deg == 0) {
		gen0(len);
		return 0;
	}

	charm = charm_create(deg);
	no_files = argc - 3;
	if ((files = malloc(no_files * sizeof(*files))) == NULL) {
		fprintf(stderr, "malloc\n");
		return -1;
	}
	for (int i = 3; i < argc; i++) {
		files[i - 3] = fopen(argv[i], "r");
		if (!files[i - 3]) {
			fprintf(stderr, "failed to open file \"%s\"\n", argv[i]);
			return i - 2;
		}
	}

	generate(len, files, no_files, deg);
	return 0;
}
