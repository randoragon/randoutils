# Lindenmayer System

A small program to compute L-System strings after N iterations. The ruleset is hard-coded and can be changed manually at will.

- all rules must map a single character to a string
- 2 parameters on STDIN: initial string and how many iterations

Prints out the resulting string.

## Requirements

[RND\_HashMap](https://github.com/randoragon/randoutils/tree/master/c-libs/hashmap)

## Compilation

```sh
gcc -lrnd_hashmap lsystem.c
```
