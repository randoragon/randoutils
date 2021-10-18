#include "utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


static unsigned task_num = 0;
static unsigned subtask_num = 0;

void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);
	exit(1);
}

int randi(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

double randf(double min, double max)
{
	return min + ((double)rand() / RAND_MAX * (max - min));
}

size_t choose(const double *prob, size_t n)
{
	size_t ret = 0;
	double accumulator = prob[0],
		sum = 0.0,
		pick;

	for (const double *f = prob; f < prob + n; f++) {
		sum += *f;
		if (!isfinite(sum))
			die("floating point error");
	}

	pick = randf(0.0, sum);
	for (const double *f = prob + 1; f < prob + n; f++) {
		if (pick < accumulator)
			return ret;
		accumulator += *f;
		ret++;
	}
	return n - 1;
}

FILE *openr(const char *fname)
{
	FILE *file;
	file = fopen(fname, "r");
	if (file == NULL) {
		die("failed to read file '%s'", fname);
	}
	return file;
}

void task(const char *caption)
{
	if (task_num != 0) {
		printf("\npress enter to continue... ");
		getchar();
	}
	printf("\n===[ %u. %s ]===\n", ++task_num, caption);
	subtask_num = 0;
}

void subtask(const char *caption)
{
	if (subtask_num != 0) {
		printf("\npress enter to continue... ");
		getchar();
	}
	printf("\n---[ %u.%u. %s ]---\n", task_num, ++subtask_num, caption);
}

void *allocate(size_t nmemb, size_t size)
{
	void *ret;
	ret = calloc(nmemb, size);
	if (ret == NULL) {
		die("out of memory (calloc)");
	}
	return ret;
}
