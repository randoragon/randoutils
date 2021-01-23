CC=gcc
CFLAGS=-std=c99 -Wpedantic -Werror

all: main

main:
	@printf "This is just a header file, no need to compile\n"

install:
	cp -- RND_ErrMsg.h /usr/include/

clean:

example: example.o
	$(CC) $(CFLAGS) example.c -o a.out
