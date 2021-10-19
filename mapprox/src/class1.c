#include "class1.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include "utils.h"

void gen0(unsigned len)
{
	const char *alphabet = "abcdefghijklmnopqrstuvwxyz ";
	const size_t maxi = strlen(alphabet) - 1;
	unsigned counter = 0,
		 no_words = 0;
	bool was_space = true;

	for (unsigned i = 0; i < len; i++) {
		const char c = alphabet[randi(0, maxi)];
		if (c == ' ') {
			was_space = true;
		} else {
			if (was_space)
				no_words++;
			counter++;
			was_space = false;
		}
		putchar(c);
	}

	printf("\nNo. Words: %u\nAverage Word Length: %lg\n",
			no_words, (no_words == 0 ? 0 : (double)counter / no_words));
}

size_t c2idx(char c)
{
	if (c >= 'a' && c <= 'z') {
		return c - 'a';
	} else if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= '0' && c <= '9') {
		return c - '0' + 26;
	} else if (isspace(c)) {
		return 36;
	}
	return SIZE_MAX;
}

char idx2c(size_t idx)
{
	if (idx < 26) {
		return 'a' + idx;
	} else if (idx < 36) {
		return '0' + idx - 26;
	} else if (idx == 36) {
		return ' ';
	}
	die("illegal index");
	return -1;
}

void *charm_create(size_t degree)
{
	if (degree == 1)
		return allocate(CHARM_LEN, sizeof(unsigned));

	void **const ret = allocate(CHARM_LEN, sizeof(*ret));

	for (size_t i = 0; i < CHARM_LEN; i++)
		ret[i] = charm_create(degree - 1);

	return ret;
}

void charm_incr(void *charm, const size_t *idx, size_t degree)
{
	if (degree == 1) {
		((unsigned*)charm)[*idx]++;
		return;
	}

	charm_incr(((unsigned**)charm)[*idx], idx + 1, degree - 1);
}

unsigned charm_get(const void *charm, const size_t *idx, size_t degree)
{
	if (degree == 1)
		return ((unsigned*)charm)[*idx];

	return charm_get(((unsigned**)charm)[*idx], idx + 1, degree - 1);
}

void charm_destroy(void *charm, size_t degree)
{
	if (degree > 1)
		for (size_t i = 0; i < CHARM_LEN; i++)
			charm_destroy(((void**)charm)[i], degree - 1);
	free(charm);
}

void count_chars(FILE *input, void *charm, size_t degree)
{
	/* hist will store the history of last {degree} characters */
	char *const hist = allocate(degree, sizeof(*hist));

	/* Used later for caching */
	size_t *const idx = allocate(degree, sizeof(*idx));

	int c;

	if (fseek(input, 0L, SEEK_SET) < 0) {
		perror("fseek failed");
		exit(1);
	}

	/* The first {degree-1} history slots need to be populated
	 * before the algorithm can be applied. */
	for (size_t i = 0; i < degree - 1; i++)
		if ((hist[1 + i] = fgetc(input)) == EOF)
			return;

	while ((c = fgetc(input)) != EOF) {
		/* Advance history by 1 character */
		for (size_t i = 0; i < degree - 1; i++)
			hist[i] = hist[i + 1];
		hist[degree - 1] = c;

		/* Convert characters to charm indices.
		 * If illegal characters exist in history, skip evaluation */
		for (size_t i = 0; i < degree; i++) {
			idx[i] = c2idx(hist[i]);
			if (idx[i] == SIZE_MAX)
				goto skip;
		}

		/* Increment no. exact occurrences */
		charm_incr(charm, idx, degree);

		if (degree > 1) {
			/* Increment no. occurrences with any ending */
			idx[degree - 1] = CHARM_LEN - 1;
			charm_incr(charm, idx, degree);
		}

		/* Increment no. all total occurrences */
		idx[0] = CHARM_LEN - 1;
		memset(idx + 1, 0x00, (degree - 1) * sizeof(*idx));
		charm_incr(charm, idx, degree);

skip:		;
	}

	/* Cleanup */
	free(idx);
	free(hist);
}

void generate_init(size_t len, const void *charm, const void *charm1, size_t degree, const char *init_str)
{
	unsigned counter = 0,
		 no_words = 0;
	bool was_space = true;

	/* Cache for speed */
	const size_t init_str_len = strlen(init_str);

	/* Used for addressing in charm functions */
	size_t *const idx = allocate(degree, sizeof(*idx));

	/* To minimize memory footprint and keep things simple, narrower
	 * probabilities will be recalculated each iteration instead of all of
	 * them being cached simultaneously. Fortunately, doing {CHARM_LEN-1}
	 * calculations per iteration is not a very bad price to pay. */
	double probs[CHARM_LEN - 1];

	/* History buffer for remembering previous characters */
	char *const hist = allocate(degree, sizeof(*hist));


	if (init_str_len < degree - 1)
		die("init_str is too short");

	strcpy(hist, init_str + (init_str_len - degree + 1));

	for (unsigned i = 0; i < len; i++) {

		/* Calculate probabilities for this narrow case */
		for (size_t j = 0; j < CHARM_LEN - 1; j++) {
			unsigned prob_whole, prob_prefix;

			for (size_t k = 0; k < degree - 1; k++) {
				idx[k] = c2idx(hist[k]);
				if (idx[k] == SIZE_MAX)
					die("illegal character in init_str");
			}

			/* Skip denominators for probabilities, because of later
			 * division */
			idx[degree - 1] = j;
			prob_whole  = charm_get(charm, idx, degree);
			idx[degree - 1] = CHARM_LEN - 1;
			prob_prefix = charm_get(charm, idx, degree);

			probs[j] = (prob_whole == 0 || prob_prefix == 0) ? 0.0 : ((double)prob_whole / prob_prefix);
			if (!isfinite(probs[j]))
				die("probability float error (%u / %u = %lf)", prob_whole, prob_prefix, probs[j]);
		}

		/* Choose character according to probabilities */
		bool is_unknown = true;
		for (size_t j = 0; j < CHARM_LEN - 1; j++)
			if (probs[j] != 0.0) {
				is_unknown = false;
				break;
			}
		if (is_unknown)
			/* This means the specific string of characters did not
			 * appear anywhere within the training data. Select
			 * according to 1st order from charm1 then. */
			for (size_t j = 0; j < CHARM_LEN - 1; j++) {
				const size_t idx[] = { j };
				probs[j] = charm_get(charm1, idx, 1);
			}
		const char c = idx2c(choose(probs, CHARM_LEN - 1));

		if (c == ' ') {
			was_space = true;
		} else {
			if (was_space)
				no_words++;
			counter++;
			was_space = false;
		}
		putchar(c);

		/* Update history */
		if (degree > 1) {
			memmove(hist, hist + 1, (degree - 2) * sizeof(*hist));
			hist[degree - 2] = c;
		}
	}

	/* Cleanup */
	free(idx);
	free(hist);

	printf("\nNo. Words: %u\nAverage Word Length: %lg\n",
			no_words, (no_words == 0 ? 0 : (double)counter / no_words));
}

void sgenerate_init(char *output, size_t len, const void *charm, const void *charm1, size_t degree, const char *init_str)
{
	/* Cache for speed */
	const size_t init_str_len = strlen(init_str);

	/* Used for addressing in charm functions */
	size_t *const idx = allocate(degree, sizeof(*idx));

	/* To minimize memory footprint and keep things simple, narrower
	 * probabilities will be recalculated each iteration instead of all of
	 * them being cached simultaneously. Fortunately, doing {CHARM_LEN-1}
	 * calculations per iteration is not a very bad price to pay. */
	double probs[CHARM_LEN - 1];

	/* History buffer for remembering previous characters */
	char *const hist = allocate(degree, sizeof(*hist));


	if (init_str_len < degree - 1)
		die("init_str is too short");

	strcpy(hist, init_str + (init_str_len - degree + 1));

	for (size_t i = 0; i < len; i++) {

		/* Calculate probabilities for this narrow case */
		for (size_t j = 0; j < CHARM_LEN - 1; j++) {
			unsigned prob_whole, prob_prefix;

			for (size_t k = 0; k < degree - 1; k++) {
				idx[k] = c2idx(hist[k]);
				if (idx[k] == SIZE_MAX)
					die("illegal character in init_str");
			}

			/* Skip denominators for probabilities, because of later
			 * division */
			idx[degree - 1] = j;
			prob_whole  = charm_get(charm, idx, degree);
			idx[degree - 1] = CHARM_LEN - 1;
			prob_prefix = charm_get(charm, idx, degree);

			probs[j] = (prob_whole == 0 || prob_prefix == 0) ? 0.0 : ((double)prob_whole / prob_prefix);
			if (!isfinite(probs[j]))
				die("probability float error (%u / %u = %lf)", prob_whole, prob_prefix, probs[j]);
		}

		/* Choose character according to probabilities */
		bool is_unknown = true;
		for (size_t j = 0; j < CHARM_LEN - 1; j++)
			if (probs[j] != 0.0) {
				is_unknown = false;
				break;
			}
		if (is_unknown)
			/* This means the specific string of characters did not
			 * appear anywhere within the training data. Select
			 * according to 1st order from charm1 then. */
			for (size_t j = 0; j < CHARM_LEN - 1; j++) {
				const size_t idx[] = { j };
				probs[j] = charm_get(charm1, idx, 1);
			}
		const char c = idx2c(choose(probs, CHARM_LEN - 1));

		sprintf(output++, "%c", c);

		/* Update history */
		if (degree > 1) {
			memmove(hist, hist + 1, (degree - 2) * sizeof(*hist));
			hist[degree - 2] = c;
		}
	}

	/* Terminate the string */
	*output = '\0';

	/* Cleanup */
	free(idx);
	free(hist);
}

void gen_init_str(char *output, FILE **files, size_t no_files, size_t degree)
{
	if (degree < 2)
		return;

	char *next = output;
	output[0] = '\0';

	void *charm1 = charm_create(1);
	for (FILE **fp = files; fp < files + no_files; fp++)
		count_chars(*fp, charm1, 1);

	for (size_t i = 1; i < degree; i++) {
		void *charm = charm_create(i);
		for (FILE **fp = files; fp < files + no_files; fp++)
			count_chars(*fp, charm, i);
		sgenerate_init(next++, 1, charm, charm1, i, output);
		*next = '\0';
		charm_destroy(charm, i);
	}

	charm_destroy(charm1, 1);
}

void generate(size_t len, FILE **files, size_t no_files, size_t degree)
{
	if (degree == 0) {
		gen0(len);
		return;
	}

	void *charm = charm_create(degree);
	void *charm1 = charm_create(1);
	char *const init = allocate(degree, sizeof(*init));

	for (FILE **fp = files; fp < files + no_files; fp++) {
		count_chars(*fp, charm, degree);
		count_chars(*fp, charm1, 1);
	}

	gen_init_str(init, files, no_files, degree);
	for (size_t i = 0; i < degree - 1 && len != 0; i++) {
		putchar(init[i]);
		if (len == 0)
			goto cleanup;
		else
			len--;
	}
	generate_init(len, charm, charm1, degree, init);

	/* Cleanup */
cleanup:
	free(init);
	charm_destroy(charm, degree);
	charm_destroy(charm1, 1);
}
