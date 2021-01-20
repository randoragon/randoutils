# BitArray

## Description

This is a small self-written library implementing a bitarray struct and related functions.
I wrote it so that I can use it in my other C projects.

## Dependencies

- [RND\_ErrMsg](https://github.com/randoragon/randoutils/tree/master/c-libs/errmsg)

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_BitArray.h>` and link with `librnd_bitarray.so` (this is usually
done by passing the `-lrnd_bitarray` flag to the compiler).

## Examples

See `example.c` file for basic usage. Refer to the source code for a complete list of functions
and other details.

