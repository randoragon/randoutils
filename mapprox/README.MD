# Markov Approximation

## Description

This is a small tool that generates n-th degree Markov approximations of
textual input. You feed the program some number of text files, it analyzes them
and computes probabilities of consecutive character substrings, and based on
that is able to generate infinite streams of text.

## Usage

	./mapprox DEGREE LENGTH [FILE...]

- `DEGREE` must be a natural number (0 included). N-th degree means that for
  each output character, N previous characters were taken into account. 0th
  degree approximation is pure randomness.
- `LENGTH` denotes the length of the output (pass -1 to get `size_t` underflow
  amount of output, which is most likely **very** large).
- `FILE` is any text file. Only A-Z, a-z, 0-9 and whitespace characters are considered,
the rest is gracefully skipped.

## Caveats

- very large input files may result in overflows/floating point errors
- if the generator encounters a string which did not appear anywhere within the
  training data, it automatically falls back to 1st degree approximation
- higher degrees require an exponential amount of memory (be careful with 5 and above)
- this is a quick project I whipped out in a few days, it hasn't been battle tested in
  a rigorous way, nor is it intended for serious use. Have fun!

## Installation

	make
