# HashMap

## Description

This is a small self-written library implementing a hashmap struct and related functions.
I wrote it so that I can use it in my other C projects.

The hashmap uses the djb2 hash function by default, you can also use your own by supplying
an appropriate function pointer at the moment of creating the hashmap. Internally, linked
lists are used to avoid name collisions (theoretical worst lookup time is O(n)).

This library depends on my other library for [linked lists](https://github.com/randoragon/randoutils/tree/master/c-libs/linkedlist).
Keep that in mind if you run into problems compiling.

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_HashMap.h>` and link with `librnd_hashmap.so` (this is usually
done by passing the `-lrnd_hashmap` flag to the compiler).

## Examples

Functions are self-explanatory if you have experience with hashmaps, so refer to the source code for details
(you can also take a look at `example.c`, it has a more elaborate example that I used for testing).
Here is the most basic use case for storing and retrieving values:

```c
RND_HashMap *map = RND_hashMapCreate(15, NULL); /* (size, hash_func) if hash_func is NULL, djb2 is used */
char *key = "motd", *val = "buy gold, bye!";
RND_hashMapAdd(map, key, val);

char *get = RND_hashMapGet(map, key);
printf("Value of key \"%s\" is \"%s\"\n", key, get);

RND_hashMapDestroy(map, NULL);
```
