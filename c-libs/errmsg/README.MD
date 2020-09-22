# ErrMsg

## Description

This is a small self-written library implementing a few printf-like functions
for printing info, warning and error messages.  I wrote it so that I can use it
in my other C projects.

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_ErrMsg.h>` and link with `librnd_errmsg.so` (this is usually
done by passing the `-lrnd_errmsg` flag to the compiler).

## Examples

The following code...

```c
RND_info("This is an info message: %d", 1);
RND_warn("This is a warning message: %d", 2);
RND_error("This is an error message: %d", 3);
```

...produces the following output:

```
[INFO]  This is an info message: 1
[WARN]  This is a warning message: 2
[ERROR] This is an error message: 3
```
