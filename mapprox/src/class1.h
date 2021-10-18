#ifndef CLASS1_H
#define CLASS1_H

#include <stdio.h>

/* DISCLAIMER
 * For many computations, I use a structure called a "character matrix",
 * abbreviated to "charm" for readability. A charm simply stores the number of
 * occurrences of each character, and a total number of all occurrences. Based
 * on that information, frequency can be calculated.
 *
 * Nth-degree approximations require N-dimensional charms.
 */

/* Char Matrix Length, consists of:
 *   26 letters a-z
 * + 10 digits  0-9
 * + 1  space   ' '
 * + 1  counter (used as denominator in calculating frequencies) */
#define CHARM_LEN 38

void gen0(unsigned len);

size_t c2idx(char c);
char   idx2c(size_t idx);

void *charm_create(size_t degree);
void charm_destroy(void *charm, size_t degree);
void charm_incr(void *charm, const size_t *idx, size_t degree);
unsigned charm_get(const void *charm, const size_t *idx, size_t degree);

void count_chars(FILE *input, void *charm, size_t degree);
void gen_init_str(char *output, FILE **files, size_t no_files, size_t degree);


/* Generate {len} characters of text with {degree}-order approximation, based on
 * probabilistic information stored in {charm}. {init_str} must be provided as a
 * starting point (must be at least {degree-1} characters long and is not
 * included in the output). The output is stored in the output buffer, which
 * must be large enough to contain {len} characters + null terminator.
 * {charm1} is a fallback charm with 1st degree probabilities. */
void sgenerate_init(char *output, size_t len, const void *charm, const void *charm1, size_t degree, const char *init_str);

/* Same as sgenerate, except prints directly to stdout instead of storing in
 * an output buffer. Also doesn't print any status info afterwards */
void generate_init(size_t len, const void *charm, const void *charm1, size_t degree, const char *init_str);

/* Generates {len} characters of text with {degree}-order approximation, based
 * on probabilistic information stored in array {files}. Each file is rewinded
 * and read in entirety. */
void generate(size_t len, FILE **files, size_t no_files, size_t degree);

#endif /* CLASS1_H */
