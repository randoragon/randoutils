# Markov Approximation

## Description

This is a small tool that generates n-th degree Markov approximations of
textual input. You feed the program some number of text files, it analyzes them
and computes probabilities of consecutive character substrings, and based on
that is able to generate infinite streams of text.

## Usage

./mapprox DEGREE LENGTH [FILE...]

DEGREE must be a natural number (0 included).  
LENGTH denotes the length of the output (pass -1 to get very, very large amounts of output).  
FILE is an simple text file. Only A-Z, a-z, 0-9 and space characters are considered.

## Caveats

- very large input files may result in overflows/floating point errors
- if the generator encounters a string which did not appear anywhere within the
  training data, it automatically falls back to 1st degree approximation
- higher degrees require an exponential amount of memory (be careful with 5 and above)
- this is a quick project I whipped out in a few days, it hasn't been battle tested in
  a rigorous way, nor is it intended for serious use. Have fun!

## Installation

	make
