# ErrMsg

## Description

This is a small self-written header file implementing a few printf-like macros
for printing info, warning and error messages. I wrote it so that I can use it
in my other C projects.

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/`.

## Usage

You only have to `#include <RND_ErrMsg.h>`. No linking, it's just a header with
a few macros.

## Examples

The following code...

```c
#include <RND_ErrMsg.h>

int main()
{
    RND_INFO("This is an info message: %d", 1);
    RND_WARN("This is a warning message: %d", 2);
    RND_ERROR("This is an error message: %d", 3);
}
```

...produces the following output:

```
 [INFO] (example.c:5) (main) This is an info message: 1
 [WARN] (example.c:6) (main) This is a warning message: 2
[ERROR] (example.c:7) (main) This is an error message: 3
```
