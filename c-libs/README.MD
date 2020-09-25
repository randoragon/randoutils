# c-libs

This directory holds a collection of my self-written C libraries.
Some of these may be data structure implementations, wrappers,
helper scripts and so on. The list may grow longer in size.
All libraries are prefixed "RND" for "Randoragon" to avoid name
collisions.

## Building

You can go into each directory individually and compile only what you need,
or you can use the makefile that resides in this directory and it will
automatically invoke the makefiles in each of the subdirectories.

Therefore, to install all libraries in this directory, run (if necessary, as root):

    make install

