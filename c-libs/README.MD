# c-libs

This directory holds a collection of my self-written C libraries.
Some of these may be data structure implementations, wrappers,
helper scripts and so on. The list may grow longer in size.
All libraries are prefixed "RND" for "Randoragon" to avoid name
collisions.

## Documentation

All libraries are throughly documented with [Doxygen](https://www.doxygen.nl).
The documentation is not included in the repository though,
you can generate it yourself with the following `make` command:

    make docs

To open the documentation, open the newly created `docs/html/index.html`
file in your internet browser of choice.

## Building

You can go into each directory individually and compile only what you need,
or you can use the makefile that resides in this directory and it will
automatically invoke the makefiles in each of the subdirectories.

Therefore, to install all libraries in this directory, run (if necessary, as root):

    make install

