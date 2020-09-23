CC=gcc
CFLAGS=-std=c99

all: main

main:
	printf "This is just a header file, no need to compile\n"

install:
	cp -- RND_ErrMsg.h /usr/include/

example: example.o
	$(CC) $(CFLAGS) example.o -o a.out
