# Stack

## Description

This is a small self-written library implementing a stack struct and related functions.
I wrote it so that I can use it in my other C projects.

### Disclaimer

This library is perfectly functional, although I don't use it anymore because linked lists perform worse than
regular arrays on modern computers due to CPU caching. That's why I set out to rewrite this library with an array
implementation and that's what I'll be using instead.

## Dependencies

- [RND\_ErrMsg](https://github.com/randoragon/randoutils/tree/master/c-libs/errmsg)

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_Stack.h>` and link with `librnd_stack.so` (this is usually
done by passing the `-lrnd_stack` flag to the compiler).

## Examples

See `example.c` file for basic usage. Refer to the source code for a complete list of functions
and other details.
