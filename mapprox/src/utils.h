/* This header includes utility functions used in other source files.
 * All these functions automatically terminate execution in case of an error,
 * which makes them convenient to use, because there's no need for manual
 * error-checking.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define LEN(X) (sizeof(X) / sizeof(*X))

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)(-1))
#endif

/* Print error message and terminate the program */
void die(const char *fmt, ...);

/* Return a random integer between min and max */
int randi(int min, int max);

/* Return a random fraction between min and max */
double randf(double min, double max);

/* Choose random element with weighted probabilities.
 * prob - array of probabilities, they don't need to be normalized
 * n    - length of prob
 * returns index of the chosen prob element. */
size_t choose(const double *prob, size_t n);

/* Open a file in read-only mode */
FILE *openr(const char *fname);

/* Print a pretty, numbered task heading */
void task(const char *caption);
void subtask(const char *caption);

/* Allocate a block of memory initialized to 0s */
void *allocate(size_t nmemb, size_t size);

#endif /* UTILS_H */
