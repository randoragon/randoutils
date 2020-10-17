# Game

## Description

Game development in C is a bit different than using a conventional language like C++,
but definitely possible and not without its own advantages. The most notable difference
is the fact that C is a procedural language, not object oriented. The aim of this library
is to implement an interface in C which benefits from the advantages of OOP while also
remaining true to the ways of doing things in pure C.

This library is not a complete solution. It will not render graphics, play sound or do any
of those things. Instead, it attempts to be a convenient wrapper for a programmer so that
they can organize features of other, lower level libraries (like SDL2) into convenient
structures and workflows.

Please refer to [OVERVIEW.MD](https://github.com/randoragon/randoutils/tree/master/c-libs/game/OVERVIEW.MD)
and the source code for more details.

## Features

- create objects and spawn instances
- manage instances (access their variables, despawn, etc.)
- write and run custom event handlers (WIP)

See [OVERVIEW.MD](https://github.com/randoragon/randoutils/tree/master/c-libs/game/OVERVIEW.MD) for
a general outline and documentation.

## Installation

You will need to install these libraries as runtime dependencies:

- [RND\_LinkedList](https://github.com/randoragon/randoutils/tree/master/c-libs/linkedlist)
- [RND\_HashMap](https://github.com/randoragon/randoutils/tree/master/c-libs/hashmap)
- [RND\_ErrMsg](https://github.com/randoragon/randoutils/tree/master/c-libs/errmsg)

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_Game.h>` and link with `librnd_game.so` (this is usually
done by passing the `-lrnd_game` flag to the compiler).

