# PriorityQueue

## Description

This is a small self-written library implementing a priority queue struct and related functions.
I wrote it so that I can use it in my other C projects.

You'll notice that the RND library suite also has a [RND\_PriorityQueueLL](https://github.com/randoragon/randoutils/tree/master/c-libs/priorityqueueLL)
library. I wrote the LL version first using linked lists - then I found out that on modern computers
you usually want to use regular arrays, as due to CPU-caching and memory locality linked lists are
almost always outperformed by them. Hence, this implementation uses the dynamic array approach.

## Dependencies

- [RND\_ErrMsg](https://github.com/randoragon/randoutils/tree/master/c-libs/errmsg)

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_PriorityQueue.h>` and link with `librnd_priorityqueue.so` (this is usually
done by passing the `-lrnd_priorityqueue` flag to the compiler).

## Examples

See `example.c` file for basic usage. Refer to the source code for a complete list of functions
and other details.
